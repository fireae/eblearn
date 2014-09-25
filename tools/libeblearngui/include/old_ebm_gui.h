#ifndef EBM_GUI_H
#define EBM_GUI_H

#include <QtGui>
#include <qpixmap.h>
#include <QResizeEvent>
#include <math.h>

#include "libeblearn.h"

#include "ui_ebbox.h"

using namespace std;
using namespace ebl;


/**
* ebbox is a general container. It can be used either as a
* base class for the specialized 1-layer modules, or as a
* container for multilayer modules
**/

class ebbox : public QGroupBox
{
	Q_OBJECT

public:
	/**
	 * The main constructor
	 * @param parent is not needed, leave it to 0
	 * @param prop is a pointer to the parameters dialog associated with the module
	 * @param vis is a pointer to the display widget
	 * @param argmutex is the mutex used to protect the idxs and other structures (derived classes) in multi-thread applications
	 * */
	ebbox(QWidget *parent = 0, const char* title = NULL, QDialog *prop =  NULL, QWidget *vis = NULL, QMutex* argmutex = NULL);
	~ebbox();

	/* *
	 * add a widget in a container usually used for display
	 * -> enables to make "multilayer" widget
	 * */
	void add_box(ebbox *ajout);

	Ui::Ebbox ui;
	QDialog *properties;
	QWidget *visible;

	/* *
	 * @param open is used to know if the display panel is open
	 * this is due to a limitation in the slot/signal system of Qt
	 *  */
	bool open;

public slots:
	//! open the properties widget "properties"
	virtual void openProperties();
	//! open and close the display widget "visible" by making it (in)visible and resizing the widget
	void resize();
	//! prints
	virtual void print();
	//! refresh the display and tells all the widgets added to do the same
	virtual void refreshform(bool propup = false);

public:
	//! not used actually but may be used in the future
	//! it is a pointer to the main widget wich could be used to send a resize() signal
	QWidget* mainwidget;

protected:
	//! this vector is used to transmit the refreshdisplay() signal to the widgets added
	vector<ebbox*> added_widgets;
	//! see constructor
	QMutex* mutex;
};


/******************************************************
* ////////////////////////////////////////////////////////////////////////////////////////////
* The next following classes handle the properties, the display
* and the main widget of the idx gui
* ////////////////////////////////////////////////////////////////////////////////////////////
*****************************************************/

/* *
 * these are the different types of idxs supported.
 * it has been created to "templatize" the display of the idxs,
 * because Qt does not allow templates of Q_OBJECTs
 * */
enum idx_type { DOUBLE, FLOAT, INTG, UBYTE };

//! structure for the display options
struct idx_plotdata
{
	double zoom, min, max;
	bool norm;
	double min_norm, max_norm;
	bool visible;
	//! special options
	bool idx1_as_array, idx3_as_RGB;
};

//! structure for the printing settings
struct idx_printdata
{
	bool pretty, elems, onfile;
	//! file is the path to the file to print on
	QString file;
};

void initialize_data(idx_plotdata*, idx_printdata*, void* , idx_type);

/*********************************************
* Idx gui  properties dialog window
*********************************************/

class idx_propgui : public QDialog
{
	Q_OBJECT

private:
	QDoubleSpinBox* zoombox;

	QRadioButton* arraybutton;
	QRadioButton* rgbbutton;

	QDoubleSpinBox* minbox;
	QDoubleSpinBox* maxbox;
	QLabel* max_normval;
	QLabel* min_normval;
	QCheckBox* visiblecheck;
	QCheckBox* normalcheck;

	QCheckBox* pretty;
	QCheckBox* elems;
	QCheckBox* print_onfile;
	QLineEdit* fileedit;

public:
	QPushButton* okbutton;
	QPushButton* cancelbutton;

public:
	idx_propgui(QWidget* parent, int order);
	~idx_propgui(){};

	//! puts the values of the different boxes of the dialog
	//! to the values of those in arguments
	void set_data(idx_plotdata* plotdata, idx_printdata* printdata);

public slots:
	//! puts the value of the arguments to those of the different boxes of the dialog
	void get_data( idx_plotdata* plotdata, idx_printdata* printdata);
	//! opens a dialog to choose wich file you want to print on
	void choose_file();

};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Idx gui displaying widget related function
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class idx_displayGui : public QWidget
{
	Q_OBJECT

public:
	/* *
	*  display contains the pointers to the QLabels in which the different layers of the idx will be displayed
	* */
	vector<QLabel*> display;
	QScrollArea* scroll;
	QWidget* displayer;
	QHBoxLayout* hlayout;

	idx_displayGui(QWidget* parent, int NofDisplay = 1, const char* title = "Idx");
	~idx_displayGui(){};

	//! handles the display of all the layers into the different QLabels
	//! according to the display options and settings
	void displayAll(void* idx, idx_type type, idx_plotdata* plotdata);

private:
	/* *
	 * display into the @param container the @param indexofLayer -th layer of the Idx @param matrix idx of the @param type idx_type,
	 * normalizing between @param vmin and @param vmax, and sizing it with @param zoom
	 * @param idx1_as_array : if true, the Idx1 is displayed as an array (1pixel-high), if not it is displayed as a plot
	 * @param id3_as_rgb: if true, the layers are grouped by 3 and displayed as RGB
	 * */
	static void displaymatrixlayer(QLabel* container, void* matrix, idx_type type, int indexofLayer = 0, double vmin = -1, double vmax = 1, double zoom = 1, bool idx1_as_array = false, bool idx3_as_rgb = false);
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  idx gui related function
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

class Idx_Gui : public ebbox
{
	Q_OBJECT

private:
	idx_plotdata plotdata;
	idx_printdata printdata;

	/* *
	 * an Idx is memorized as a void* pointer to the idx and an idx_type to know how to cast it properly when needed (display)
	 * this is to templatize the idx_gui
	 * */
	idx_type type;
	void* idx;

	/* *
	 * @param fromproperties is used to know if we refresh the display from
	 * the properties dialog (so we have to update the plotting datas) or from
	 * the keyboard shortcuts (the plotting datas are already changed)
	 * anyway as a user you don't have to use it
	 * */
	bool fromproperties;

	//! opens the properties dialog
	void openProperties();

	QString checkproperties(QString s);

public slots:
	/* *
	 * checks various display settings, triggers a warning if it seems weird,
	 *  and calls the displayAll() function of the display widget
	 * */
	void refreshdisplay();

	//! prints accordingly to the print settings
	void print();

	/* *
	 * handles the keyboard event. So far:
	 * = (equals) -> zoom in
	 * - (minus) -> zoom out
	 * p -> print
	 * q -> close
	 * */
	void keyPressEvent( QKeyEvent* event);

public:
	Idx_Gui(void* idxarg, idx_type argtype, const char* title = "idx", QWidget *parent = 0, QMutex* mutex = NULL);
	~Idx_Gui(){};

	//! sets the min value for custom normalisation
	void setvmin(double min);
	//! sets the max value for custoom normalisation
	void setvmax(double max);
	//! enable (or not) the visualization
	void setvisible(bool visible);
	//! set visualization as array if idx1
	void setidx1_as_array(bool array);
	//! set visualization as RGB if idx3
	void setidx3_as_RGB(bool rgb);
};

/******************************************************
* ////////////////////////////////////////////////////////////////////////////////////////////
* The next following classes handle the properties, the display
* and the main widget of the state_idx gui
* ////////////////////////////////////////////////////////////////////////////////////////////
*****************************************************/

//! structure for the display options
struct state_idx_data
{
	idx_plotdata x_plotdata, dx_plotdata, ddx_plotdata;
	idx_printdata x_printdata, dx_printdata, ddx_printdata;
};

/*********************************************
* state_Idx gui  properties dialog window
*********************************************/

class state_idx_propgui : public QDialog
{
	Q_OBJECT

private:
	QDoubleSpinBox* zoombox;

	QRadioButton* arraybutton;
	QRadioButton* rgbbutton;

	QDoubleSpinBox* X_minbox;
	QDoubleSpinBox* X_maxbox;
	QLabel* X_minnorm;
	QLabel* X_maxnorm;
	QDoubleSpinBox* DX_minbox;
	QDoubleSpinBox* DX_maxbox;
	QLabel* DX_minnorm;
	QLabel* DX_maxnorm;
	QDoubleSpinBox* DDX_minbox;
	QDoubleSpinBox* DDX_maxbox;
	QLabel* DDX_minnorm;
	QLabel* DDX_maxnorm;

	QCheckBox* X_visiblecheck;
	QCheckBox* X_normalcheck;
	QCheckBox* DX_visiblecheck;
	QCheckBox* DX_normalcheck;
	QCheckBox* DDX_visiblecheck;
	QCheckBox* DDX_normalcheck;

	QCheckBox* X_pretty;
	QCheckBox* X_elems;
	QCheckBox* DX_pretty;
	QCheckBox* DX_elems;
	QCheckBox* DDX_pretty;
	QCheckBox* DDX_elems;
	QCheckBox* print_onfile;
	QLineEdit* fileedit;

public:
	QPushButton* okbutton;
	QPushButton* cancelbutton;

public:
	state_idx_propgui(QWidget* parent, int order);
	~state_idx_propgui(){};

	//! puts the values of the different boxes of the dialog
	// ! to the values of those in arguments
	void set_data(state_idx_data* data);


public slots:
	//! puts the value of the arguments to those of the different boxes of the dialog
	void get_data( state_idx_data* data);
	//! opens a dialog to choose wich file you want to print on
	void choose_file();

};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * state_Idx gui displaying widget related function
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class state_idx_displayGui : public QWidget
{
	Q_OBJECT

public:

	idx_displayGui* x_displayer;
	idx_displayGui* dx_displayer;
	idx_displayGui* ddx_displayer;

	state_idx_displayGui(QWidget* parent, int x_NofDisplay = 1);
	~state_idx_displayGui(){};

	//! handles the display of all the layers into the different QLabels
	//! according to the display options and settings
	void displayAll(state_idx* state, state_idx_data* data);

};


/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  state_idx gui related function
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

class state_Idx_Gui : public ebbox
{
	Q_OBJECT

private:
	state_idx_data data;
	state_idx* state;

	/* *
	 * @param fromproperties is used to know if we refresh the display from
	 * the properties dialog (so we have to update the plotting datas) or from
	 * the keyboard shortcuts (the plotting datas are already changed)
	 * anyway as a user you don't have to use it
	 * */
	bool fromproperties;

	//! opens the properties dialog
	void openProperties();

public slots:
	/* *
	 * checks various display settings, triggers a warning if it seems weird,
	 *  and calls the displayAll() function of the display widget
	 * */
	void refreshdisplay();

	//! prints accordingly to the print settings
	void print();

	/* *
	 * handles the keyboard event. So far:
	 * = (equals) -> zoom in
	 * - (minus) -> zoom out
	 * p -> print
	 * q -> close
	 * */
	void keyPressEvent( QKeyEvent* event);

public:
	state_Idx_Gui(state_idx* stateIdx, const char* title = "state_idx", QWidget *parent = 0, QMutex* mutex = NULL);
	~state_Idx_Gui(){};

	//! sets the min value for custom normalisation
	void setvmin(double xmin, double dxmin, double ddxmin);
	//! sets the max value for custoom normalisation
	void setvmax(double xmax, double dxmax, double ddxmax);
	//! enable (or not) the visualization
	void setvisible(bool xvisible, bool dxvisible, bool ddxvisible);
	//! set visualization as array if idx1
	void setidx1_as_array(bool xarray, bool dxarray, bool ddxarray);
	//! set visualization as RGB if idx3
	void setidx3_as_RGB(bool xrgb, bool dxrgb, bool ddxrgb);
};


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ////////////////////////////////////////////////////////////////////////////////////////////
 * From this point, the next classes are specialized gui
 * designed for the corresponding ebm modules
 * ////////////////////////////////////////////////////////////////////////////////////////////
 *
 * All the constructors are doubled
 * The first version creates automatically state_idx_GUIs for
 * the state_idxs provided, and then creates the other GUIs
 * that are special to the module
 * The second only creates the later
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* *
 * General module_1_1 GUI: use it for every module_1_1 that doesn't have any particular feature to display
 * Keep in my mind that this GUI library only displays things, you won't be able to launch any operation (for now at least)
 * this module is to be used for :
 *		the stdsigmoid_module
 * 		the tanh_module
 * for the rest of the specialized module_1_1, use the specialized GUIs
 * */
class module_1_1_GUI : public ebbox
{
	Q_OBJECT

private:
	state_idx* in;
	state_idx* out;

public:
	module_1_1_GUI(state_idx* in, state_idx* out, const char* title = "module_1_1", QWidget *parent = 0, QMutex* mutex = NULL);
	module_1_1_GUI(const char* title = "module_1_1", QWidget *parent = 0, QMutex* mutex = NULL);
	~module_1_1_GUI(){};
};

/* * * * * * * * * * * * * * * * * * * * * * *
 * GUI for module_2_1 (2 ins and 1 out)
 * * * * * * * * * * * * * * * * * * * * * * */
class module_2_1_GUI : public ebbox
{
	Q_OBJECT

private:
	state_idx* in1;
	state_idx* in2;
	state_idx* out;

public:
	module_2_1_GUI(state_idx* in1, state_idx* in2, state_idx* out, const char* title = "module_2_1", QWidget *parent = 0, QMutex* mutex = NULL);
	module_2_1_GUI(const char* title = "module_2_1", QWidget *parent = 0, QMutex* mutex = NULL);
	~module_2_1_GUI(){};
};

/* * * * * * * * * * * * * * * * * * * * * * *
 * GUI for layers_2 (general architecture with one hidden layer)
 * The GUI automatically provides :
 * 		a state_idx_GUI for the hidden layer
 * 		2 module_1_1_GUI for the 2 modules
 * * * * * * * * * * * * * * * * * * * * * * */
class layers_2_GUI : public ebbox
{
	Q_OBJECT

private:
	layers_2<state_idx, state_idx, state_idx>* layer2;
	state_idx* in;
	state_idx* out;

public:
	layers_2_GUI(layers_2<state_idx, state_idx, state_idx>* layer2, state_idx* in, state_idx* out, const char* title = "layers_2 <state_idx, state_idx, state_idx>", QWidget *parent = 0, QMutex* mutex = NULL);
	layers_2_GUI(layers_2<state_idx, state_idx, state_idx>* layer2, const char* title = "layers_2 <state_idx, state_idx, state_idx>", QWidget *parent = 0, QMutex* mutex = NULL);
	~layers_2_GUI(){};
};

/* * * * * * * * * * * * * * * * * * * * * * *
 * GUI for fc_ebm1 (1 in EBM)
 * The GUI automatically provides :
 * 		a state_idx_GUI for the out calculated by the function
 * 		2 module_1_1_GUI for the 2 modules
 * * * * * * * * * * * * * * * * * * * * * * */
class fc_ebm1_GUI : public ebbox
{
	Q_OBJECT

private:
	fc_ebm1<state_idx, state_idx>* ebm1;
	state_idx* in;
	state_idx* energy;

public:
	fc_ebm1_GUI(fc_ebm1<state_idx, state_idx>* ebm1, state_idx* in, state_idx* energy, const char* title = "fc_ebm1<state_idx, state_idx>", QWidget *parent = 0, QMutex* mutex = NULL);
	fc_ebm1_GUI(fc_ebm1<state_idx, state_idx>* ebm1, const char* title = "fc_ebm1<state_idx, state_idx>", QWidget *parent = 0, QMutex* mutex = NULL);
	~fc_ebm1_GUI(){};
};

/* * * * * * * * * * * * * * * * * * * * * * *
 * GUI for fc_ebm2 (2 ins EBM)
 * The GUI automatically provides :
 * 		a state_idx_GUI for the out calculated by the function
 * 		2 module_1_1_GUI for the 2 modules
 * * * * * * * * * * * * * * * * * * * * * * */
class fc_ebm2_GUI : public ebbox
{
	Q_OBJECT

private:
	fc_ebm2<state_idx, state_idx, state_idx>* ebm2;
	state_idx* in1;
	state_idx* in2;
	state_idx* energy;

public:
	fc_ebm2_GUI(fc_ebm2<state_idx, state_idx, state_idx>* ebm2, state_idx* in1, state_idx* in2, state_idx* energy, const char* title = "fc_ebm2<state_idx, state_idx, state_idx>", QWidget *parent = 0, QMutex* mutex = NULL);
	fc_ebm2_GUI(fc_ebm2<state_idx, state_idx, state_idx>* ebm2, const char* title = "fc_ebm2<state_idx, state_idx, state_idx>", QWidget *parent = 0, QMutex* mutex = NULL);
	~fc_ebm2_GUI(){};
};

/* * * * * * * * * * * * * * * * * * * * * * *
 * GUI for linear modules
 * The GUI automatically provides a state_idx_GUI for the weights
 * * * * * * * * * * * * * * * * * * * * * * */
class linear_module_GUI : public module_1_1_GUI
{
	Q_OBJECT

private:
	linear_module* module;

public:
	linear_module_GUI(linear_module* module, state_idx *in, state_idx *out, const char* title = "linear module", QWidget *parent = 0, QMutex* mutex = NULL);
	linear_module_GUI(linear_module* module, const char* title = "linear module", QWidget *parent = 0, QMutex* mutex = NULL);
	~linear_module_GUI(){};
};

/* * * * * * * * * * * * * * * * * * * * * * *
 * GUI for full_layer (fully-connected neural net layer: linear + tanh non-linearity)
 * The GUI automatically provides:
 * 		a linear_module_GUI
 * 	 	Two state_idx_GUI for the bias and the sum state_idxs
 * 		a module_1_1_GUI for the tan_h module
 * * * * * * * * * * * * * * * * * * * * * * */
class full_layer_GUI : public module_1_1_GUI
{
	Q_OBJECT

private:
	full_layer* layer;

public:
	full_layer_GUI(full_layer* layer, state_idx *in, state_idx *out, const char* title = "full_layer", QWidget *parent = 0, QMutex* mutex = NULL);
	full_layer_GUI(full_layer* layer, const char* title = "full_layer", QWidget *parent = 0, QMutex* mutex = NULL);
	~full_layer_GUI(){};
};

/* * * * * * * * * * * * * * * * * * * * * * *
 * GUI for f_layerl (full connection between replicable 3D layers)
 * The GUI automatically provides:
 * 	 	Three state_idx_GUI for the weights, bias and sum state_idxs
 * 		a module_1_1_GUI for the squash function
 * * * * * * * * * * * * * * * * * * * * * * */
class f_layer_GUI : public module_1_1_GUI
{
	Q_OBJECT

private:
	f_layer* layer;

public:
	f_layer_GUI(f_layer* layer, state_idx *in, state_idx *out, const char* title = "f_layer", const char* squash_title = "squash", QWidget *parent = 0, QMutex* mutex = NULL);
	f_layer_GUI(f_layer* layer, const char* title = "f_layer", const char* squash_title = "squash", QWidget *parent = 0, QMutex* mutex = NULL);
	~f_layer_GUI(){};
};

/* * * * * * * * * * * * * * * * * * * * * * *
 * GUI for c_layer (convolutional layer module)
 * The GUI automatically provides:
 * 		an Idx_GUI for the table
 * 	 	Three state_idx_GUI for the kernel, bias and sum state_idxs
 * 		a module_1_1_GUI for the squash function
 * * * * * * * * * * * * * * * * * * * * * * */
class c_layer_GUI : public module_1_1_GUI
{
	Q_OBJECT

private:
	c_layer* layer;

public:
	c_layer_GUI(c_layer* layer, state_idx *in, state_idx *out, const char* title = "c_layer", const char* squash_title = "squash function", QWidget *parent = 0, QMutex* mutex = NULL);
	c_layer_GUI(c_layer* layer, const char* title = "c_layer", const char* squash_title = "squash", QWidget *parent = 0, QMutex* mutex = NULL);
	~c_layer_GUI(){};
};

/* * * * * * * * * * * * * * * * * * * * * * *
 * GUI for s_layer (subsampling layer class)
 * The GUI automatically provides:
 * 	 	4 state_idx_GUI for the coeff, bias sub and sum state_idxs
 * 		a module_1_1_GUI for the squash function
 * * * * * * * * * * * * * * * * * * * * * * */
class s_layer_GUI : public module_1_1_GUI
{
	Q_OBJECT

private:
	s_layer* layer;

public:
	s_layer_GUI(s_layer* layer, state_idx *in, state_idx *out, const char* title = "s_layer", const char* squash_title = "squash function", QWidget *parent = 0, QMutex* mutex = NULL);
	s_layer_GUI(s_layer* layer, const char* title = "s_layer", const char* squash_title = "squash", QWidget *parent = 0, QMutex* mutex = NULL);
	~s_layer_GUI(){};
};

/* * * * * * * * * * * * * * * * * * * * * * *
 * GUI for logadd_layer (log-add over spatial dimensions of an idx3-state
 * The GUI automatically provides 2 Idx_GUIs for the expdist and sumexp Idxs
 * * * * * * * * * * * * * * * * * * * * * * */
class logadd_layer_GUI : public module_1_1_GUI
{
	Q_OBJECT

private:
	logadd_layer* layer;

public:
	logadd_layer_GUI(logadd_layer* layer, state_idx *in, state_idx *out, const char* title = "logadd_layer", QWidget *parent = 0, QMutex* mutex = NULL);
	logadd_layer_GUI(logadd_layer* layer, const char* title = "logadd_layer", QWidget *parent = 0, QMutex* mutex = NULL);
	~logadd_layer_GUI(){};
};

/* * * * * * * * * * * * * * * * * * * * * * *
 * GUI for edist_cost (replicable Euclidean distance cost function.)
 * The GUI automatically provides :
 * 		a logadd_layer_GUI
 * 		two state_idx_GUI for the distance and loggaded distance
 * 		1 Idx_GUI for the proto
 * * * * * * * * * * * * * * * * * * * * * * */
class edist_cost_GUI : public module_1_1_GUI
{
	Q_OBJECT

private:
	edist_cost* edist;

public:
	edist_cost_GUI(edist_cost* edist, state_idx *in, state_idx *energy, const char* title = "edist_cost", QWidget *parent = 0, QMutex* mutex = NULL);
	edist_cost_GUI(edist_cost* edist, const char* title = "edist_cost", QWidget *parent = 0, QMutex* mutex = NULL);
	~edist_cost_GUI(){};
};


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ebwindow is a simple "whiteboard" on which you can display
 * idxs with gray_draw_matrix and RGB_draw_matrix
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
class ebwindow : public QWidget
{
	Q_OBJECT

private:
	QPixmap* mydisplay;
	QLabel* mylabel;
	QPainter* painter;

	void resizeEvent (QResizeEvent *event);


public:
	/* *
	 * Be careful to create a whiteboard big enough for your pictures, since you won't be able
	 * to make it bigger after (ie resizing the window will scale the content, you won't have more space to draw on)
	 * */
	ebwindow(int height = 500, int width = 500);

	/* *
	 * gray_draw_matrix displays your idx2 or the first layer of your idx3 in grayscale on the whiteboard.
	 * This function does a copy of your idx and won't change in in any way !
	 * @param idx and @param type are, like before, used to templatize the function
	 * @param x and @param y are the coordinates of the top-left corner of your picture on the whiteboard
	 * @param minv and @param maxv are the min and max values to set colors. If left to zero, the min of
	 * 			your idx will be set to 0 and the max will be 255
	 * @param zoomx and @param zoomy are the zoom factors in width and height
	 * @param mutex is used if you want to protect your idx (multi-thread)
	 * */
	void gray_draw_matrix(void* idx, idx_type type, int x = 0, int y = 0, int minv = 0, int maxv = 0, int zoomx = 1, int zoomy = 1, QMutex* mutex = NULL);

	/* *
	 * RGB_draw_matrix displays the 3 firsts layers of your idx3 as a RGB picture on the whiteboard.
	 * Attention : it won't change the values in your idx, so if you want a good display, you have to make it
	 * 			an idx3 with values between 0 and 255 !!
	 * This function does a copy of your idx and won't change in in any way !
	 * @param idx and @param type are, like before, used to templatize the function
	 * @param x and @param y are the coordinates of the top-left corner of your picture on the whiteboard
	 * @param zoomx and @param zoomy are the zoom factors in width and height
	 * @param mutex is used if you want to protect your idx (multi-thread)
	 * */
	void RGB_draw_matrix(void* idx, idx_type type, int x = 0, int y = 0, int zoomx = 1, int zoomy = 1, QMutex* mutex = NULL);
};

#endif // EBM_GUI_H


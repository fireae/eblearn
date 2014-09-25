#include "ebm_gui.h"

////////////////////////////////////////////////////////////////////////////////////////////
// eb_box related methods
////////////////////////////////////////////////////////////////////////////////////////////

ebbox::ebbox(QWidget *parent, const char* title, QDialog *prop, QWidget *vis, QMutex* argmutex)
	: QGroupBox(title, parent)
{
	properties = prop;
	visible = vis;
	mutex = argmutex;
	open = false;
	ui.setupUi(this, title);
	if(visible != NULL) ui.vboxLayoutcont->addWidget(visible);
	ui.subcontainer->setVisible(false);
	mainwidget = NULL;
	QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setSizePolicy(policy);

	connect( ui.pushButton_expand, SIGNAL( clicked() ), this, SLOT( resize() ) );
	connect( ui.pushButton_print, SIGNAL( clicked() ), this, SLOT( print() ) );
	connect( ui.pushButton_properties, SIGNAL( clicked() ), this, SLOT( openProperties() ) );

	show();
}

ebbox::~ebbox()
{
	delete mutex;
}

void ebbox::resize()
{
   	if( open ) {
   		ui.subcontainer->setVisible(false);
   		open = false;
   	}
   	else {
   		ui.subcontainer->setVisible(true);
   		open = true;
   	}
   	refreshform(true);
}

void ebbox::openProperties()
{
	if(properties != NULL){
		properties->setModal(true);
		properties->show();
	}
}

void ebbox::print(){}

void ebbox::add_box(ebbox *ajout)
{
	ui.vboxLayoutcont->addWidget(ajout);
	added_widgets.push_back(ajout);
	if(mainwidget != NULL) ajout->mainwidget = mainwidget;
	else ajout->mainwidget = this;
}

void ebbox::refreshform(bool propup)
{
   	if((mainwidget != NULL)&&propup){
   		dynamic_cast<ebbox*>(mainwidget)->refreshform();
   	}
   	else{
   		adjustSize();
   		for(int i = 0; i < (int)added_widgets.size(); i++) added_widgets[i]->refreshform();
   	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Idx gui  properties window related functions
////////////////////////////////////////////////////////////////////////////////////////////

void initialize_data(idx_plotdata* plotdata, idx_printdata* printdata, void* idx, idx_type type){
	plotdata->zoom = 1;
	plotdata->norm = true;
	plotdata->visible = true;
	if(type == DOUBLE){
		plotdata->min = idx_min(*(static_cast<Idx<double>*>(idx)));
		plotdata->max = idx_max(*(static_cast<Idx<double>*>(idx)));
		plotdata->min_norm = idx_min(*(static_cast<Idx<double>*>(idx)));
		plotdata->max_norm = idx_max(*(static_cast<Idx<double>*>(idx)));
	}
	if(type == FLOAT){
		plotdata->min = idx_min(*(static_cast<Idx<float>*>(idx)));
		plotdata->max = idx_max(*(static_cast<Idx<float>*>(idx)));
		plotdata->min_norm = idx_min(*(static_cast<Idx<float>*>(idx)));
		plotdata->max_norm = idx_max(*(static_cast<Idx<float>*>(idx)));
	}
	if(type == INTG){
		plotdata->min = idx_min(*(static_cast<Idx<intg>*>(idx)));
		plotdata->max = idx_max(*(static_cast<Idx<intg>*>(idx)));
		plotdata->min_norm = idx_min(*(static_cast<Idx<intg>*>(idx)));
		plotdata->max_norm = idx_max(*(static_cast<Idx<intg>*>(idx)));
	}
	if(type == UBYTE){
		plotdata->min = idx_min(*(static_cast<Idx<ubyte>*>(idx)));
		plotdata->max = idx_max(*(static_cast<Idx<ubyte>*>(idx)));
		plotdata->min_norm = idx_min(*(static_cast<Idx<ubyte>*>(idx)));
		plotdata->max_norm = idx_max(*(static_cast<Idx<ubyte>*>(idx)));
	}

	plotdata->idx1_as_array = false;
	plotdata->idx3_as_RGB = false;

	printdata->pretty = true;
	printdata->elems = false;
	printdata->onfile = false;
}

idx_propgui::idx_propgui(QWidget* parent, int order):
	QDialog(parent)
{

	//setFixedSize(350, 600);
	setWindowTitle("Idx Properties");

	QVBoxLayout* vmainLayout = new QVBoxLayout();
	setLayout(vmainLayout);

	// zoom box
	QWidget* zoomwidget = new QWidget();
	QHBoxLayout* hzoomLayout = new QHBoxLayout();
	zoombox = new QDoubleSpinBox(this);
	hzoomLayout->addWidget(new QLabel("Zoom factor : ", this));
	hzoomLayout->addWidget(zoombox);
	zoomwidget->setLayout(hzoomLayout);
	vmainLayout->addWidget(zoomwidget);

	// Idx1 special properties box. Added only for Idx-1
	QWidget* idx1widget = new QWidget();
	QHBoxLayout* hidx1Layout = new QHBoxLayout();
	hidx1Layout->addWidget(new QLabel("Display Idx1 as :", 0));
	QRadioButton* plotbutton = new QRadioButton("a plot", idx1widget);
	plotbutton->setChecked(true);
	arraybutton = new QRadioButton("an array", idx1widget);
	hidx1Layout->addWidget(plotbutton);
	hidx1Layout->addWidget(arraybutton);
	idx1widget->setLayout(hidx1Layout);
	if(order == 1) vmainLayout->addWidget(idx1widget);

	// Idx3 special properties. Added only for Idx-3 or more
	QWidget* idx3widget = new QWidget();
	QHBoxLayout* hidx3Layout = new QHBoxLayout();
	hidx3Layout->addWidget(new QLabel("Display Idx3 in :", 0));
	QRadioButton* graybutton = new QRadioButton("grayscale (1 layer)", idx3widget);
	graybutton->setChecked(true);
	rgbbutton = new QRadioButton("RGB (3 layers)", idx3widget);
	hidx3Layout->addWidget(graybutton);
	hidx3Layout->addWidget(rgbbutton);
	idx3widget->setLayout(hidx3Layout);
	if(order >= 3) vmainLayout->addWidget(idx3widget);


	QGroupBox* groupbox = new QGroupBox("display properties");
	QGridLayout* gLayout = new QGridLayout();
	groupbox->setLayout(gLayout);

	maxbox = new QDoubleSpinBox(this);
	maxbox->setMaximum(999.99);
	gLayout->addWidget(new QLabel("     max : ", this), 0,0);
	gLayout->addWidget(maxbox, 0, 1);
	minbox = new QDoubleSpinBox(this);
	minbox->setMinimum(-999.99);
	gLayout->addWidget(new QLabel("     min : ", this), 1, 0);
	gLayout->addWidget(minbox, 1, 1);

	min_normval = new QLabel();
	min_normval->setEnabled(false);
	gLayout->addWidget(min_normval, 1, 2);
	max_normval = new QLabel();
	max_normval->setEnabled(false);
	gLayout->addWidget(max_normval, 0, 2);

	visiblecheck = new QCheckBox("visibility");
	gLayout->addWidget(visiblecheck, 0 , 3);
	normalcheck = new QCheckBox("normalize");
	gLayout->addWidget(normalcheck, 1, 3);

	vmainLayout->addWidget(groupbox);

	connect(normalcheck, SIGNAL(toggled(bool)), minbox, SLOT(setDisabled(bool)));
	connect(normalcheck, SIGNAL(toggled(bool)), maxbox, SLOT(setDisabled(bool)));


	QGroupBox* printbox = new QGroupBox("Printing properties");
	QGridLayout* printLayout = new QGridLayout();
	printbox->setLayout(printLayout);

	pretty = new QCheckBox("Pretty print");
	printLayout->addWidget(pretty, 0, 0);

	elems = new QCheckBox("print elems");
	printLayout->addWidget(elems, 0, 2);

	print_onfile = new QCheckBox("print on file : (default : print on console)");
	printLayout->addWidget(print_onfile, 1, 0, 1, 3);
	fileedit = new QLineEdit();
	fileedit->setEnabled(false);
	printLayout->addWidget(fileedit, 2,0, 1, 2);
	QPushButton* filebutton = new QPushButton("select file...");
	filebutton->setEnabled(false);
	printLayout->addWidget(filebutton, 2,2);

	connect(print_onfile, SIGNAL(toggled(bool)), fileedit, SLOT(setEnabled(bool)));
	connect(print_onfile, SIGNAL(toggled(bool)), filebutton, SLOT(setEnabled(bool)));
	connect(filebutton, SIGNAL(clicked()), this, SLOT( choose_file()));

	vmainLayout->addWidget(printbox);

	///////////////////////////////////////////////////////
	// OK and cancel button
	//////////////////////////////////////////////////////
	QHBoxLayout* hbuttonLayout = new QHBoxLayout();
	okbutton = new QPushButton("OK", this);
	hbuttonLayout->addWidget(okbutton);
	cancelbutton = new QPushButton("Cancel", this);
	hbuttonLayout->addWidget(cancelbutton);
	vmainLayout->addLayout(hbuttonLayout);

	connect( cancelbutton, SIGNAL( clicked() ), this, SLOT( reject() ) );

}

void idx_propgui::set_data(idx_plotdata* plotdata, idx_printdata* printdata)
{
	zoombox->setValue(plotdata->zoom);

	arraybutton->setChecked(plotdata->idx1_as_array);
	rgbbutton->setChecked(plotdata->idx3_as_RGB);

	minbox->setValue(plotdata->min);
	maxbox->setValue(plotdata->max);
	min_normval->setNum((double)plotdata->min_norm);
	max_normval->setNum((double)plotdata->max_norm);
	visiblecheck->setChecked(plotdata->visible);
	normalcheck->setChecked(plotdata->norm);

	pretty->setChecked(printdata->pretty);
	elems->setChecked(printdata->elems);
	print_onfile->setChecked(printdata->onfile);
	fileedit->setText(printdata->file);
}

void idx_propgui::get_data(idx_plotdata* plotdata, idx_printdata* printdata)
{
	plotdata->zoom = zoombox->value();

	plotdata->idx1_as_array = arraybutton->isChecked();
	plotdata->idx3_as_RGB = rgbbutton->isChecked();

	plotdata->min = minbox->value();
	plotdata->max = maxbox->value();
	plotdata->visible = visiblecheck->isChecked();
	plotdata->norm = normalcheck->isChecked();

	printdata->pretty = pretty->isChecked();
	printdata->elems = elems->isChecked();
	printdata->onfile = print_onfile->isChecked();
	printdata->file = fileedit->text();
}

void idx_propgui::choose_file()
{
	QString fichier = QFileDialog::getOpenFileName(this, "Choose file as output", QString());
	fileedit->setText(fichier);
}


////////////////////////////////////////////////////////////////////////////
// Idx gui displaying widget related function
///////////////////////////////////////////////////////////////////////////

idx_displayGui::idx_displayGui(QWidget* parent, int NofDisplay,const char* title):
	QWidget(parent)
{
	QVBoxLayout* mainlayout = new QVBoxLayout();
	mainlayout->setMargin(1);
	setLayout(mainlayout);

	QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	// scrolling display area for X
	displayer = new QWidget();
	hlayout = new QHBoxLayout();
	displayer->setLayout(hlayout);
	displayer->setSizePolicy(policy);
	scroll = new QScrollArea();
	scroll->setWidget(displayer);
	scroll->setSizePolicy(policy);

	mainlayout->addWidget(scroll);

	QLabel *labeltemp;

	// QLabels containers for displaying the different layers of X
	QLabel* label = new QLabel(title);
	label->setScaledContents(true);
	label->setSizePolicy(policy);
	hlayout->addWidget(label);
	for(int i = 0; i < NofDisplay; i++){
		labeltemp = new QLabel();
		labeltemp->setScaledContents(true);
		hlayout->addWidget(labeltemp);
		display.push_back(labeltemp);
	}

	setSizePolicy(policy);
}

void idx_displayGui::displaymatrixlayer(QLabel* container, void* matrix, idx_type type, int indexofLayer, double vmin, double vmax, double zoom, bool idx1_as_array, bool idx3_as_rgb)
{
	int dim0 = static_cast<Idx<double>*>(matrix)->dim(0);
	int dim1 = static_cast<Idx<double>*>(matrix)->dim(1);
	int dim2 = static_cast<Idx<double>*>(matrix)->dim(2);
	int order = static_cast<Idx<double>*>(matrix)->order();

	// if we try to access to a non-existant layer, error
	if((order > 2) && (indexofLayer > dim0-1) ) eblerror("non-existant layer");

	// if it is an Idx-1 and idx1_as_array is false, we plot it
	if(order == 1 && !idx1_as_array){
		QPixmap displaymap(dim0*zoom, 30*zoom);
		displaymap.fill(Qt::white);
		QPainter painter(&displaymap);
		for(int i = 0; i < dim0; i++){
			int pointvalue;
			double value_double;
			float value_float;
			intg value_intg;
			int value_ubyte;
			// here we calculate the position of the point
			switch((int)type){
				case 0:
					value_double = static_cast<Idx<double>*>(matrix)->get(i);
					if(value_double >= 0) pointvalue = (int)floor(30 - (vmax - value_double)/(vmax-vmin)*30);
					else pointvalue = (int) floor((value_double - vmin)/(vmax-vmin)*30);
					if(value_double > vmax) pointvalue = 30;
					if(value_double < vmin) pointvalue = 0;
					break;
				case 1:
					value_float = static_cast<Idx<float>*>(matrix)->get(i);
					if(value_float >= 0) pointvalue = (int)floor(30 - (vmax - value_float)/(vmax-vmin)*30);
					else pointvalue = (int) floor((value_float - vmin)/(vmax-vmin)*30);
					if(value_float > vmax) pointvalue = 30;
					if(value_float < vmin) pointvalue = 0;
					break;
				case 2:
					value_intg = static_cast<Idx<intg>*>(matrix)->get(i);
					if(value_intg >= 0) pointvalue = (int)floor(30 - (vmax - value_intg)/(vmax-vmin)*30);
					else pointvalue = (int) floor((value_intg - vmin)/(vmax-vmin)*30);
					if(value_intg > vmax) pointvalue = 30;
					if(value_intg < vmin) pointvalue = 0;
					break;
				case 3:
					value_ubyte = (int)static_cast<Idx<ubyte>*>(matrix)->get(i);
					if(value_ubyte >= 0) pointvalue = (int)floor(30 - (vmax - value_ubyte)/(vmax-vmin)*30);
					else pointvalue = (int) floor((value_intg - vmin)/(vmax-vmin)*30);
					if(value_ubyte > vmax) pointvalue = 30;
					if(value_ubyte < vmin) pointvalue = 0;
					break;
			}

			// If the zoom is higher than 2, we make bigger pixels...
			for(int j= -floor(zoom/2); j <= floor(zoom/2); j++){
				for(int k= -floor(zoom/2); k <= floor(zoom/2); k++){
					if((i*zoom+j >= 0) && ((i*zoom+j) <= dim0*zoom -1) && ((pointvalue*zoom+k) >= 0) && ((pointvalue*zoom+k) <= 30*zoom - 1)){
						painter.drawPoint(i*zoom+j, pointvalue*zoom+k);
					}
				}
			}
		}
		// then we put it into the container and resize everything accordingly
		container->setPixmap(displaymap);
		container->adjustSize();
		container->setMinimumSize(dim0*zoom+5, 30*zoom +2);
		QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		container->setSizePolicy(policy);
		container->setFrameShape(QFrame::Box);
	}

	// else, if idx3_as_rgb is false,  we display the layer as a grayscale matrix
	else if(( idx1_as_array) || (order == 2) || ((order >= 3) && !idx3_as_rgb)){
		if(idx1_as_array) dim1 = 1;
		// the dimensions x and y in the picture are swapped to match the i and j dimensions in Idxs

		Idx<ubyte>* mycopy = (order < 3)? new Idx<ubyte>(dim0, dim1) : new Idx<ubyte>(dim1, dim2);
		if(type == DOUBLE){
			Idx<double> test1(mycopy->dim(0), mycopy->dim(1));
			if(order==2) idx_copy(*(static_cast<Idx<double>*>(matrix)), test1);
			else {
				Idx<double> bla = static_cast<Idx<double>*>(matrix)->select(0,indexofLayer);
				idx_copy( bla, test1);
			}
			idx_dotc( test1, (double)(255/(vmax-vmin)), test1);
			idx_addc( test1, (double)(-vmin*255/(vmax-vmin)), test1);
			idx_copy( test1, *mycopy);
		}
		if(type == FLOAT){
			Idx<float> test2(mycopy->dim(0), mycopy->dim(1));
			if(order==2) idx_copy(*(static_cast<Idx<float>*>(matrix)), test2);
			else {
				Idx<float> bla = static_cast<Idx<float>*>(matrix)->select(0,indexofLayer);
				idx_copy( bla, test2);
			}
			idx_dotc( test2, (float)(255/(vmax-vmin)), test2);
			idx_addc( test2, (float)(-vmin*255/(vmax-vmin)), test2);
			idx_copy( test2, *mycopy);
		}
		if(type == INTG){
			if(order==2) idx_copy(*(static_cast<Idx<intg>*>(matrix)), *mycopy);
			else {
				Idx<intg> test3 = static_cast<Idx<intg>*>(matrix)->select(0,indexofLayer);
				idx_copy(test3, *mycopy);
			}
			idx_dotc(*mycopy, (ubyte)(255/(vmax-vmin)), *mycopy);
			idx_addc(*mycopy, (ubyte)(-vmin*255/(vmax-vmin)), *mycopy);
		}
		if(type == UBYTE){
			if(order==2) idx_copy(*(static_cast<Idx<ubyte>*>(matrix)), *mycopy);
			else {
				Idx<ubyte> test4 = static_cast<Idx<ubyte>*>(matrix)->select(2,indexofLayer);
				idx_copy(test4, *mycopy);
			}
			idx_dotc(*mycopy, (ubyte)(255/(vmax-vmin)), *mycopy);
			idx_addc(*mycopy, (ubyte)(-vmin*255/(vmax-vmin)), *mycopy);
		}

	    QPixmap* mydisplay = new QPixmap(mycopy->dim(1), mycopy->dim(0));
	    QPainter* painter = new QPainter(mydisplay);

		QImage* image;
		QVector<QRgb> table(256);
		for (int i = 0; i < 256; i++){
		    table[i] = qRgb(i, i, i);
		}
		image = new QImage((uchar*)mycopy->idx_ptr(), mycopy->dim(1), mycopy->dim(0), mycopy->dim(1) * sizeof(unsigned char), QImage::Format_Indexed8);
		image->setColorTable(table);
		//image = new QImage(image->scaled(dim1*zoom, dim0*zoom));
		painter->drawImage(0, 0, *image);

	    //container->setGeometry(QRect(0,0, dim1*zoom, dim0*zoom));
	    container->setPixmap(*mydisplay);

		container->setMinimumSize(mycopy->dim(1)*zoom + 2, mycopy->dim(0)*zoom + 2);
		QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		container->setSizePolicy(policy);
		container->setFrameShape(QFrame::Box);
	}
	// if the order of the Idx is equal or more than 3 and the Idx3_as_rgb is true, we group the layers 3 by 3 and make it RGB matrices
	else{
		Idx<ubyte>myimage(dim1, dim2, 4);
		Idx<ubyte> mypartimage = myimage.narrow(2,3,0);
		if(type == DOUBLE){
			Idx<double> test1 = static_cast<Idx<double>*>(matrix)->narrow(0,3,indexofLayer);
			{ idx_aloop2(i, test1, double, o, mypartimage, ubyte){
				*o = (ubyte)((*i - vmin)/(vmax-vmin) * 255);
				if(*o >= 255) *o = 255;
				if(*o <= 0) *o = 0;
				}
			}
		}
		if(type == FLOAT){
			Idx<float> test2 = static_cast<Idx<float>*>(matrix)->narrow(0,3,indexofLayer);
			{ idx_aloop2(i, test2, float, o, mypartimage, ubyte){
				*o = (ubyte)((*i - vmin)/(vmax-vmin) * 255);
				if(*o >= 255) *o = 255;
				if(*o <= 0) *o = 0;
				}
			}
		}
		if(type == INTG){
			Idx<intg> test3 = static_cast<Idx<intg>*>(matrix)->narrow(2,3,indexofLayer);
			idx_copy(test3, mypartimage);
			idx_dotc(myimage, (ubyte)(255/(vmax-vmin)), myimage);
			idx_addc(myimage, (ubyte)(-vmin*255/(vmax-vmin)), myimage);
		}
		if(type == UBYTE){
			Idx<ubyte> test4 = static_cast<Idx<ubyte>*>(matrix)->narrow(2,2,indexofLayer);
			idx_copy(test4, mypartimage);
			idx_dotc(myimage, (ubyte)(255/(vmax-vmin)), myimage);
			idx_addc(myimage, (ubyte)(-vmin*255/(vmax-vmin)), myimage);
		}

	    QPixmap* mydisplay = new QPixmap(dim2, dim1);
	    QPainter* painter = new QPainter(mydisplay);

		QImage* image;
		image = new QImage((uchar*)myimage.idx_ptr(), dim2, dim1, dim2 * sizeof(unsigned char), QImage::Format_RGB32);
		image = new QImage(image->rgbSwapped());
		painter->drawImage( 0, 0, *image);

	    container->setGeometry(QRect(0,0, dim2*zoom, dim1*zoom));
	    container->setPixmap(*mydisplay);

		container->setMinimumSize(dim2*zoom + 2, dim1*zoom + 2);
		QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		container->setSizePolicy(policy);
		container->setFrameShape(QFrame::Box);

	}
}

void idx_displayGui::displayAll(void* idx, idx_type type, idx_plotdata* plotdata)
{
	// if idx3_as_RGB is false, one container will display a unique layer
	if(!(plotdata->idx3_as_RGB)){
		int Noflayer = display.size();
		displayer->setMinimumWidth(30);
		// create the images for each layer of the idx
		for(int i =0; i < Noflayer; i++){
			displaymatrixlayer(display[i], idx, type, i, plotdata->norm ? plotdata->min_norm : plotdata->min, plotdata->norm ? plotdata->max_norm : plotdata->max, plotdata->zoom, plotdata->idx1_as_array, false);
			displayer->setMinimumWidth(displayer->minimumWidth() + display[i]->minimumWidth() + 2*(plotdata->zoom+1));
			display[i]->setVisible(true);
		}
		// sets the size of the various containers for X
		displayer->setMinimumHeight(display[0]->minimumHeight()+10);
		displayer->setMaximumSize(displayer->minimumSize());
		scroll->setMinimumHeight( displayer->minimumHeight() + 25);
		scroll->setMinimumWidth(min(displayer->minimumWidth() + 25, 450));
		if(Noflayer == 1) scroll->setFixedWidth(min(displayer->minimumWidth() + 25, 800));
		scroll->setVisible(plotdata->visible);

		adjustSize();
	}
	else {
		// if idx3_as_RGB is true, only 1/3 of the containers will be used : we have to check that there still are 3 layers to display as RGB or display the 1 or 2 remaining in grayscale, then
		// disable the remaining containers
		int Noflayer = display.size();
		displayer->setMinimumWidth(30);
		int currentplot = 0, i=0;
		while(i < Noflayer){
			// if enough layers in X to make a RGB matrix
			if( i + 2 < Noflayer){
				displaymatrixlayer(display[currentplot], idx, type, i, plotdata->norm ? plotdata->min_norm : plotdata->min, plotdata->norm ? plotdata->max_norm : plotdata->max, plotdata->zoom, false, true);
				displayer->setMinimumWidth(displayer->minimumWidth() + display[currentplot]->minimumWidth()+ 2*(plotdata->zoom+1));
				currentplot++;
				i = i+3;
			}
			else{
				// if not enough layers in X, make grayscale matrix
				displaymatrixlayer(display[currentplot], idx, type, i, plotdata->norm ? plotdata->min_norm : plotdata->min, plotdata->norm ? plotdata->max_norm : plotdata->max, plotdata->zoom, false, false);
				displayer->setMinimumWidth(displayer->minimumWidth() + display[currentplot]->minimumWidth()+ 2*(plotdata->zoom+1));
				currentplot++;
				i++;
			}
		}
		// then disable the remaining containers for X
		for(int j = currentplot; j < Noflayer; j++) {
			display[j]->setVisible(false);
		}
		// finally, resize the various containers
		displayer->setMinimumHeight(display[0]->minimumHeight()+10);
		displayer->setMaximumSize(displayer->minimumSize());
		scroll->setMinimumHeight(displayer->minimumHeight() + 25);
		//scroll->setMaximumWidth(displayer->minimumWidth() + 25);
		scroll->setVisible(plotdata->visible);
	}
}

////////////////////////////////////////////////////////////////////////////
// idx gui related function
///////////////////////////////////////////////////////////////////////////
Idx_Gui::Idx_Gui(void* idxarg, idx_type argtype, const char* title, QWidget *parent, QMutex* mutex):
	ebbox(parent, title, NULL, NULL, mutex)
{
	idx = idxarg;
	type = argtype;
	if(idx == NULL) {
		eblerror("Idx Null");
		return;
	}
	if(static_cast<Idx<double>*>(idx)->order() == 0) {
		eblerror("Not defined for Idx0");
		return;
	}

	initialize_data(&plotdata, &printdata, idx, type);

	fromproperties = false;

	properties = new idx_propgui(0, static_cast<Idx<double>*>(idx)->order());
	visible = new idx_displayGui(0, ((static_cast<Idx<double>*>(idx)->order() > 2) && (static_cast<Idx<double>*>(idx)->dim(0) >1)) ? static_cast<Idx<double>*>(idx)->dim(0) : 1, title);
	ui.vboxLayoutcont->addWidget(visible);
	dynamic_cast<idx_displayGui*>(visible)->displayAll(idx, type, &plotdata);
	connect(dynamic_cast<idx_propgui*>(properties)->okbutton, SIGNAL( clicked() ), this, SLOT(refreshdisplay() ) );
}

void Idx_Gui::openProperties()
{
	dynamic_cast<idx_propgui*>(properties)->set_data(&plotdata, &printdata);
	fromproperties = true;
	properties->setModal(true);
	properties->show();
}

void Idx_Gui::refreshdisplay()
{
	refreshform();

	if(fromproperties){
		// if the display settings are sets from the properties dialog, we have to update it (contrary to the case where it is sets by keyboard shortcuts)
		dynamic_cast<idx_propgui*>(properties)->get_data(&plotdata, &printdata);
		fromproperties = false;
	}
	if(mutex != NULL) mutex->lock();
	QString errors;
	errors = checkproperties(errors);
	int validate;
	if(errors != NULL){
		errors = "We have detected the following problems : \n" + errors + "\n";
		errors = errors + "Do you still want to validate these properties ?";
		validate = QMessageBox::warning(this, "Properties checking", errors, QMessageBox::Yes | QMessageBox::No);
	} else validate = QMessageBox::Yes;
	if(validate == QMessageBox::Yes){
		dynamic_cast<idx_displayGui*>(visible)->displayAll(idx, type, &plotdata);
		properties->close();
		setFocus();
		adjustSize();
	}
	if(mutex != NULL) mutex->unlock();
}

QString Idx_Gui::checkproperties(QString s){
	// checks various possibilities for wrong or strange display settings
	if((plotdata.max <= plotdata.min) && !(plotdata.norm) && plotdata.visible) s = s + "the max is inferior or equal to the min; ";
	if((plotdata.min > plotdata.min_norm) && !(plotdata.norm) && plotdata.visible) s = s +  "the min is superior to the minimum of the idx; ";
	if((plotdata.max < plotdata.max_norm) && !(plotdata.norm) && plotdata.visible) s = s +  "the max is inferior to the maximum of the idx; ";
	return s;
}

void Idx_Gui::print()
{
	if(mutex != NULL) mutex->lock();
	if(!(printdata.onfile)){
		switch(type){
			case 0:
				if(printdata.pretty) { printf(" Idx : \n"); static_cast<Idx<double>*>(idx)->pretty(stdout);}
				if(printdata.elems) { printf("elements : \n"); static_cast<Idx<double>*>(idx)->printElems();}
				break;
			case 1:
				if(printdata.pretty) { printf(" Idx : \n"); static_cast<Idx<float>*>(idx)->pretty(stdout);}
				if(printdata.elems) { printf("elements : \n"); static_cast<Idx<float>*>(idx)->printElems();}
				break;
			case 2:
				if(printdata.pretty) { printf(" Idx : \n"); static_cast<Idx<intg>*>(idx)->pretty(stdout);}
				if(printdata.elems) { printf("elements : \n"); static_cast<Idx<intg>*>(idx)->printElems();}
				break;
			case 3:
				if(printdata.pretty) { printf(" Idx : \n"); static_cast<Idx<ubyte>*>(idx)->pretty(stdout);}
				if(printdata.elems) { printf("elements : \n"); static_cast<Idx<ubyte>*>(idx)->printElems();}
				break;
		}

	}

	else {
		ofstream stream;
		stream.open( printdata.file.toAscii());

		switch(type){
			case 0:
				if(printdata.pretty) { stream <<  "Idx : \n" ; static_cast<Idx<double>*>(idx)->pretty(stream); }
				if(printdata.elems) { stream << "elements : \n"; static_cast<Idx<double>*>(idx)->printElems(stream); stream << "\n";}
				break;
			case 1:
				if(printdata.pretty) { stream <<  "Idx : \n" ; static_cast<Idx<float>*>(idx)->pretty(stream); }
				if(printdata.elems) { stream << "elements : \n"; static_cast<Idx<float>*>(idx)->printElems(stream); stream << "\n";}
				break;
			case 2:
				if(printdata.pretty) { stream <<  "Idx : \n" ; static_cast<Idx<intg>*>(idx)->pretty(stream); }
				if(printdata.elems) { stream << "elements : \n"; static_cast<Idx<intg>*>(idx)->printElems(stream); stream << "\n";}
				break;
			case 3:
				if(printdata.pretty) { stream <<  "Idx : \n" ; static_cast<Idx<ubyte>*>(idx)->pretty(stream); }
				if(printdata.elems) { stream << "elements : \n"; static_cast<Idx<ubyte>*>(idx)->printElems(stream); stream << "\n";}
				break;
		}

		stream.close();
	}
	if(mutex != NULL) mutex->unlock();
}

void Idx_Gui::keyPressEvent( QKeyEvent* event){
	if(event->key() == Qt::Key_Equal){
		plotdata.zoom++;
		refreshdisplay();
	}
	if(event->key() == Qt::Key_Minus){
		int validate;
		if(plotdata.zoom == 1) validate = QMessageBox::warning(this, "properties checking", "The zoom will be set to 0. Are you sure ?", QMessageBox::Yes | QMessageBox::No);
		else validate = QMessageBox::Yes;
		if(validate == QMessageBox::Yes){
			plotdata.zoom--;
			refreshdisplay();
		}
	}
	if(event->key() == Qt::Key_P){
		print();
	}
	if(event->key() == Qt::Key_Q){
		close();
	}
}

void Idx_Gui::setvmin(double min){
	plotdata.min = min;
	plotdata.norm = false;
	dynamic_cast<idx_displayGui*>(visible)->displayAll(idx, type, &plotdata);
}

void Idx_Gui::setvmax(double max){
	plotdata.max = max;
	plotdata.norm = false;
	dynamic_cast<idx_displayGui*>(visible)->displayAll(idx, type, &plotdata);
}

void Idx_Gui::setvisible(bool isvisible){
	plotdata.visible = isvisible;
	refreshdisplay();
}

void Idx_Gui::setidx1_as_array(bool array){
	if(static_cast<Idx<double>*>(idx)->order() == 1){
		plotdata.idx1_as_array = array;
		refreshdisplay();
	}
}

void Idx_Gui::setidx3_as_RGB(bool rgb){
	if(static_cast<Idx<double>*>(idx)->order() >= 3){
		plotdata.idx3_as_RGB = rgb;
		refreshdisplay();
	}
}


////////////////////////////////////////////////////////////////////////////
// state_Idx gui  properties window related functions
///////////////////////////////////////////////////////////////////////////

state_idx_propgui::state_idx_propgui(QWidget* parent, int order):
	QDialog(parent)
{
	QTabWidget* tabs = new QTabWidget();

	//setFixedSize(350, 600);
	setWindowTitle("Idx Properties");

	QVBoxLayout* vmainLayout = new QVBoxLayout();
	setLayout(vmainLayout);


	//////////////////////////////////////////////
	// display tab
	/////////////////////////////////////////////

	QWidget* displaytab = new QWidget();
	QVBoxLayout* displayLayout = new QVBoxLayout();
	displaytab->setLayout(displayLayout);

	// zoom box
	QWidget* zoomwidget = new QWidget();
	QHBoxLayout* hzoomLayout = new QHBoxLayout();
	zoombox = new QDoubleSpinBox(this);
	hzoomLayout->addWidget(new QLabel("Zoom factor : ", this));
	hzoomLayout->addWidget(zoombox);
	zoomwidget->setLayout(hzoomLayout);
	displayLayout->addWidget(zoomwidget);

	// Idx1 special properties box. Added only for Idx-1
	QWidget* idx1widget = new QWidget();
	QHBoxLayout* hidx1Layout = new QHBoxLayout();
	hidx1Layout->addWidget(new QLabel("Display Idx1 as :", 0));
	QRadioButton* plotbutton = new QRadioButton("a plot", idx1widget);
	plotbutton->setChecked(true);
	arraybutton = new QRadioButton("an array", idx1widget);
	hidx1Layout->addWidget(plotbutton);
	hidx1Layout->addWidget(arraybutton);
	idx1widget->setLayout(hidx1Layout);
	if(order == 1) displayLayout->addWidget(idx1widget);

	// Idx3 special properties. Added only for Idx-3 or more
	QWidget* idx3widget = new QWidget();
	QHBoxLayout* hidx3Layout = new QHBoxLayout();
	hidx3Layout->addWidget(new QLabel("Display Idx3 in :", 0));
	QRadioButton* graybutton = new QRadioButton("grayscale (1 layer)", idx3widget);
	graybutton->setChecked(true);
	rgbbutton = new QRadioButton("RGB (3 layers)", idx3widget);
	hidx3Layout->addWidget(graybutton);
	hidx3Layout->addWidget(rgbbutton);
	idx3widget->setLayout(hidx3Layout);
	if(order >= 3) displayLayout->addWidget(idx3widget);


	// X box
	QGroupBox* Xgroupbox = new QGroupBox("X properties");
	QGridLayout* gxLayout = new QGridLayout();
	Xgroupbox->setLayout(gxLayout);

	X_maxbox = new QDoubleSpinBox(this);
	X_maxbox->setMaximum(999.99);
	gxLayout->addWidget(new QLabel("     X max : ", this), 0, 0);
	gxLayout->addWidget(X_maxbox, 0, 1);
	X_minbox = new QDoubleSpinBox(this);
	X_minbox->setMinimum(-999.99);
	gxLayout->addWidget(new QLabel("     X min : ", this), 1, 0);
	gxLayout->addWidget(X_minbox, 1, 1);

	X_maxnorm = new QLabel();
	X_maxnorm->setEnabled(false);
	gxLayout->addWidget(X_maxnorm, 0, 2);
	X_minnorm = new QLabel();
	X_minnorm->setEnabled(false);
	gxLayout->addWidget(X_minnorm, 1, 2);

	X_visiblecheck = new QCheckBox("visibility");
	gxLayout->addWidget(X_visiblecheck, 0,3);
	X_normalcheck = new QCheckBox("normalize");
	gxLayout->addWidget(X_normalcheck, 1, 3);

	displayLayout->addWidget(Xgroupbox);

	connect(X_normalcheck, SIGNAL(toggled(bool)), X_minbox, SLOT(setDisabled(bool)));
	connect(X_normalcheck, SIGNAL(toggled(bool)), X_maxbox, SLOT(setDisabled(bool)));


	//DX box
	QGroupBox* DXgroupbox = new QGroupBox("dX properties");
	QGridLayout* gdxLayout = new QGridLayout();
	DXgroupbox->setLayout(gdxLayout);

	DX_maxbox = new QDoubleSpinBox(this);
	DX_maxbox->setMaximum(999.99);
	gdxLayout->addWidget(new QLabel("   dX max : ", this), 0, 0);
	gdxLayout->addWidget(DX_maxbox, 0, 1);
	DX_minbox = new QDoubleSpinBox(this);
	DX_minbox->setMinimum(-999.99);
	gdxLayout->addWidget(new QLabel("   dX min : ", this), 1, 0);
	gdxLayout->addWidget(DX_minbox, 1, 1);

	DX_maxnorm = new QLabel();
	DX_maxnorm->setEnabled(false);
	gdxLayout->addWidget(DX_maxnorm, 0, 2);
	DX_minnorm = new QLabel();
	DX_minnorm->setEnabled(false);
	gdxLayout->addWidget(DX_minnorm, 1, 2);

	DX_visiblecheck = new QCheckBox("visibility");
	gdxLayout->addWidget(DX_visiblecheck, 0,3);
	DX_normalcheck = new QCheckBox("normalize");
	gdxLayout->addWidget(DX_normalcheck, 1, 3);

	displayLayout->addWidget(DXgroupbox);

	connect(DX_normalcheck, SIGNAL(toggled(bool)), DX_minbox, SLOT(setDisabled(bool)));
	connect(DX_normalcheck, SIGNAL(toggled(bool)), DX_maxbox, SLOT(setDisabled(bool)));

	//DDX box
	QGroupBox* DDXgroupbox = new QGroupBox("ddX properties");
	QGridLayout* gddxLayout = new QGridLayout();
	DDXgroupbox->setLayout(gddxLayout);

	DDX_maxbox = new QDoubleSpinBox(this);
	DDX_maxbox->setMaximum(999.99);
	gddxLayout->addWidget(new QLabel("  ddX max : ", this), 0, 0);
	gddxLayout->addWidget(DDX_maxbox, 0, 1);
	DDX_minbox = new QDoubleSpinBox(this);
	DDX_minbox->setMinimum(-999.99);
	gddxLayout->addWidget(new QLabel("  ddX min : ", this), 1, 0);
	gddxLayout->addWidget(DDX_minbox, 1, 1);

	DDX_maxnorm = new QLabel();
	DDX_maxnorm->setEnabled(false);
	gddxLayout->addWidget(DDX_maxnorm, 0, 2);
	DDX_minnorm = new QLabel();
	DDX_minnorm->setEnabled(false);
	gddxLayout->addWidget(DDX_minnorm, 1, 2);

	DDX_visiblecheck = new QCheckBox("visibility");
	gddxLayout->addWidget(DDX_visiblecheck, 0,3);
	DDX_normalcheck = new QCheckBox("normalize");
	gddxLayout->addWidget(DDX_normalcheck, 1, 3);

	displayLayout->addWidget(DDXgroupbox);

	connect(DDX_normalcheck, SIGNAL(toggled(bool)), DDX_minbox, SLOT(setDisabled(bool)));
	connect(DDX_normalcheck, SIGNAL(toggled(bool)), DDX_maxbox, SLOT(setDisabled(bool)));

	tabs->addTab(displaytab, "display properties");


	/////////////////////////////////////////////////////////////
	// print tab
	////////////////////////////////////////////////////////////
	QWidget* printtab = new QWidget();

	QGridLayout* printLayout = new QGridLayout();
	printtab->setLayout(printLayout);

	printLayout->addWidget(new QLabel("pretty print :"), 0, 0);
	X_pretty = new QCheckBox("X");
	printLayout->addWidget(X_pretty, 1, 0);
	DX_pretty = new QCheckBox("dX");
	printLayout->addWidget(DX_pretty, 1,1);
	DDX_pretty = new QCheckBox("ddX");
	printLayout->addWidget(DDX_pretty, 1,2);

	printLayout->addWidget(new QLabel("print elems :"), 2, 0);
	X_elems = new QCheckBox("X");
	printLayout->addWidget(X_elems, 3, 0);
	DX_elems = new QCheckBox("dX");
	printLayout->addWidget(DX_elems, 3,1);
	DDX_elems = new QCheckBox("ddX");
	printLayout->addWidget(DDX_elems, 3,2);

	print_onfile = new QCheckBox("print on file : (default : print on console)");
	printLayout->addWidget(print_onfile, 4, 0, 1, 3);
	fileedit = new QLineEdit();
	fileedit->setEnabled(false);
	printLayout->addWidget(fileedit, 5,0, 1, 2);
	QPushButton* filebutton = new QPushButton("select file...");
	filebutton->setEnabled(false);
	printLayout->addWidget(filebutton, 5,2);

	connect(print_onfile, SIGNAL(toggled(bool)), fileedit, SLOT(setEnabled(bool)));
	connect(print_onfile, SIGNAL(toggled(bool)), filebutton, SLOT(setEnabled(bool)));
	connect(filebutton, SIGNAL(clicked()), this, SLOT( choose_file()));

	printLayout->setMargin(40);

	tabs->addTab(printtab, "printing properties");

	vmainLayout->addWidget(tabs);

	///////////////////////////////////////////////////////
	// OK and cancel button
	//////////////////////////////////////////////////////
	QHBoxLayout* hbuttonLayout = new QHBoxLayout();
	okbutton = new QPushButton("OK", this);
	hbuttonLayout->addWidget(okbutton);
	cancelbutton = new QPushButton("Cancel", this);
	hbuttonLayout->addWidget(cancelbutton);
	vmainLayout->addLayout(hbuttonLayout);

	connect( cancelbutton, SIGNAL( clicked() ), this, SLOT( reject() ) );

}

void state_idx_propgui::set_data(state_idx_data* data)
{
	zoombox->setValue(data->x_plotdata.zoom);
	X_minbox->setValue(data->x_plotdata.min);
	X_maxbox->setValue(data->x_plotdata.max);
	X_maxnorm->setNum((double) data->x_plotdata.max_norm);
	X_minnorm->setNum((double) data->x_plotdata.min_norm);
	X_visiblecheck->setChecked(data->x_plotdata.visible);
	X_normalcheck->setChecked(data->x_plotdata.norm);
	arraybutton->setChecked(data->x_plotdata.idx1_as_array);
	rgbbutton->setChecked(data->x_plotdata.idx3_as_RGB);

	zoombox->setValue(data->dx_plotdata.zoom);
	DX_minbox->setValue(data->dx_plotdata.min);
	DX_maxbox->setValue(data->dx_plotdata.max);
	DX_maxnorm->setNum((double) data->dx_plotdata.max_norm);
	DX_minnorm->setNum((double) data->dx_plotdata.min_norm);
	DX_visiblecheck->setChecked(data->dx_plotdata.visible);
	DX_normalcheck->setChecked(data->dx_plotdata.norm);
	arraybutton->setChecked(data->dx_plotdata.idx1_as_array);
	rgbbutton->setChecked(data->dx_plotdata.idx3_as_RGB);

	zoombox->setValue(data->ddx_plotdata.zoom);
	DDX_minbox->setValue(data->ddx_plotdata.min);
	DDX_maxbox->setValue(data->ddx_plotdata.max);
	DDX_maxnorm->setNum((double) data->ddx_plotdata.max_norm);
	DDX_minnorm->setNum((double) data->ddx_plotdata.min_norm);
	DDX_visiblecheck->setChecked(data->ddx_plotdata.visible);
	DDX_normalcheck->setChecked(data->ddx_plotdata.norm);
	arraybutton->setChecked(data->ddx_plotdata.idx1_as_array);
	rgbbutton->setChecked(data->ddx_plotdata.idx3_as_RGB);

	X_pretty->setChecked(data->x_printdata.pretty);
	X_elems->setChecked(data->x_printdata.elems);
	print_onfile->setChecked(data->x_printdata.onfile);
	fileedit->setText(data->x_printdata.file);

	DX_pretty->setChecked(data->dx_printdata.pretty);
	DX_elems->setChecked(data->dx_printdata.elems);
	print_onfile->setChecked(data->dx_printdata.onfile);
	fileedit->setText(data->dx_printdata.file);

	DDX_pretty->setChecked(data->ddx_printdata.pretty);
	DDX_elems->setChecked(data->ddx_printdata.elems);
	print_onfile->setChecked(data->ddx_printdata.onfile);
	fileedit->setText(data->ddx_printdata.file);
}

void state_idx_propgui::get_data(state_idx_data* data)
{
	data->x_plotdata.zoom = zoombox->value();
	data->x_plotdata.min = X_minbox->value();
	data->x_plotdata.max = X_maxbox->value();
	data->x_plotdata.visible = X_visiblecheck->isChecked();
	data->x_plotdata.norm = X_normalcheck->isChecked();
	data->x_plotdata.idx1_as_array = arraybutton->isChecked();
	data->x_plotdata.idx3_as_RGB = rgbbutton->isChecked();

	data->dx_plotdata.zoom = zoombox->value();
	data->dx_plotdata.min = DX_minbox->value();
	data->dx_plotdata.max = DX_maxbox->value();
	data->dx_plotdata.visible = DX_visiblecheck->isChecked();
	data->dx_plotdata.norm = DX_normalcheck->isChecked();
	data->dx_plotdata.idx1_as_array = arraybutton->isChecked();
	data->dx_plotdata.idx3_as_RGB = rgbbutton->isChecked();

	data->ddx_plotdata.zoom = zoombox->value();
	data->ddx_plotdata.min = DDX_minbox->value();
	data->ddx_plotdata.max = DDX_maxbox->value();
	data->ddx_plotdata.visible = DDX_visiblecheck->isChecked();
	data->ddx_plotdata.norm = DDX_normalcheck->isChecked();
	data->ddx_plotdata.idx1_as_array = arraybutton->isChecked();
	data->ddx_plotdata.idx3_as_RGB = rgbbutton->isChecked();

	data->x_printdata.pretty = X_pretty->isChecked();
	data->x_printdata.elems = X_elems->isChecked();
	data->x_printdata.onfile = print_onfile->isChecked();
	data->x_printdata.file = fileedit->text();

	data->dx_printdata.pretty = DX_pretty->isChecked();
	data->dx_printdata.elems = DX_elems->isChecked();
	data->dx_printdata.onfile = print_onfile->isChecked();
	data->dx_printdata.file = fileedit->text();

	data->ddx_printdata.pretty = DDX_pretty->isChecked();
	data->ddx_printdata.elems = DDX_elems->isChecked();
	data->ddx_printdata.onfile = print_onfile->isChecked();
	data->ddx_printdata.file = fileedit->text();
}

void state_idx_propgui::choose_file()
{
	QString fichier = QFileDialog::getOpenFileName(this, "Choose file as output", QString());
	fileedit->setText(fichier);
}

////////////////////////////////////////////////////////////////////////////
// state_Idx gui displaying widget related function
///////////////////////////////////////////////////////////////////////////

state_idx_displayGui::state_idx_displayGui(QWidget* parent, int X_NofDisplay):
	QWidget(parent)
{
	QVBoxLayout* mainlayout = new QVBoxLayout();
	mainlayout->setMargin(1);
	setLayout(mainlayout);

	QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	x_displayer = new idx_displayGui(this, X_NofDisplay, "    X :");
	mainlayout->addWidget(x_displayer);

	dx_displayer = new idx_displayGui(this, X_NofDisplay, "   dX :");
	mainlayout->addWidget(dx_displayer);

	ddx_displayer = new idx_displayGui(this, X_NofDisplay, " ddX :");
	mainlayout->addWidget(ddx_displayer);

	setSizePolicy(policy);
}

void state_idx_displayGui::displayAll(state_idx* state, state_idx_data* data)
{
	x_displayer->displayAll((void*)(&state->x), DOUBLE, &(data->x_plotdata));
	dx_displayer->displayAll((void*)(&state->dx), DOUBLE, &(data->dx_plotdata));
	ddx_displayer->displayAll((void*)(&state->ddx), DOUBLE, &(data->ddx_plotdata));


}

////////////////////////////////////////////////////////////////////////////
// state_idx gui related function
///////////////////////////////////////////////////////////////////////////
state_Idx_Gui::state_Idx_Gui(state_idx *stateIdx, const char* title, QWidget *parent, QMutex* mutex):
	ebbox(parent, title, NULL, NULL, mutex)
{
	state = stateIdx;
	if(state == NULL){
		eblerror("state_idx null");
		return;
	}
	if(state->x.order() == 0) {
		eblerror("Not defined for Idx0");
		return;
	}

	initialize_data(&(data.x_plotdata), &(data.x_printdata), (void*)&(stateIdx->x), DOUBLE);
	initialize_data(&(data.dx_plotdata), &(data.dx_printdata),(void*)&(stateIdx->dx), DOUBLE);
	initialize_data(&(data.ddx_plotdata), &(data.ddx_printdata), (void*)&(stateIdx->ddx), DOUBLE);

	fromproperties = false;

	properties = new state_idx_propgui(0, state->x.order());
	visible = new state_idx_displayGui(0, ((state->x.order() > 1) && (state->x.dim(2) >1)) ? state->x.dim(2) : 1);
	ui.vboxLayoutcont->addWidget(visible);
	dynamic_cast<state_idx_displayGui*>(visible)->displayAll(state, &data);
	connect(dynamic_cast<state_idx_propgui*>(properties)->okbutton, SIGNAL( clicked() ), this, SLOT(refreshdisplay() ) );
}

void state_Idx_Gui::openProperties()
{
	dynamic_cast<state_idx_propgui*>(properties)->set_data(&data);
	fromproperties = true;
	properties->setModal(true);
	properties->show();
}

void state_Idx_Gui::refreshdisplay()
{
	refreshform();

	if(fromproperties){
		// if the display settings are sets from the properties dialog, we have to update it (contrary to the case where it is sets by keyboard shortcuts)
		dynamic_cast<state_idx_propgui*>(properties)->get_data(&data);
		fromproperties = false;
	}
	if(mutex != NULL) mutex->lock();
	QString errors;
	// checks various possibilities for wrong or strange display settings
	if((data.x_plotdata.max <= data.x_plotdata.min) && !(data.x_plotdata.norm) && data.x_plotdata.visible) errors = errors + "x max is inferior or equal to x min; ";
	if((data.dx_plotdata.max <= data.dx_plotdata.min) && !(data.dx_plotdata.norm) && data.dx_plotdata.visible) errors = errors +  "dx max is inferior or equal to dx min; ";
	if((data.ddx_plotdata.max <= data.ddx_plotdata.min) && !(data.ddx_plotdata.norm) && data.ddx_plotdata.visible) errors = errors +  "ddx max is inferior or equal to ddx min; ";
	if((data.x_plotdata.min > idx_min(state->x)) && !(data.x_plotdata.norm) && data.x_plotdata.visible) errors = errors +  "x min is superior to the minimum of the x idx; ";
	if((data.x_plotdata.max < idx_max(state->x)) && !(data.x_plotdata.norm) && data.x_plotdata.visible) errors = errors +  "x max is inferior to the maximum of the x idx; ";
	if((data.dx_plotdata.min > idx_min(state->dx)) && !(data.dx_plotdata.norm) && data.dx_plotdata.visible) errors = errors +  "dx min is superior to the minimum of the dx idx; ";
	if((data.dx_plotdata.max < idx_max(state->dx)) && !(data.dx_plotdata.norm) && data.dx_plotdata.visible) errors = errors +  "dx max is inferior to the maximum of the dx idx; ";
	if((data.ddx_plotdata.min > idx_min(state->ddx)) && !(data.ddx_plotdata.norm) && data.ddx_plotdata.visible) errors = errors +  "ddx min is superior to the minimum of the ddx idx; ";
	if((data.ddx_plotdata.max < idx_max(state->ddx)) && !(data.ddx_plotdata.norm) && data.ddx_plotdata.visible) errors = errors +  "ddx max is inferior to the maximum of the ddx idx; ";
	int validate;
	if(errors != NULL){
		errors = "We have detected the following problems : \n" + errors + "\n";
		errors = errors + "Do you still want to validate these properties ?";
		validate = QMessageBox::warning(this, "Properties checking", errors, QMessageBox::Yes | QMessageBox::No);
	} else validate = QMessageBox::Yes;
	if(validate == QMessageBox::Yes){
		dynamic_cast<state_idx_displayGui*>(visible)->displayAll(state, &data);
		properties->close();
		setFocus();
		adjustSize();
	}
	if(mutex != NULL) mutex->unlock();
}

void state_Idx_Gui::print()
{
	if(mutex != NULL) mutex->lock();
	if(!(data.x_printdata.onfile)){
		if(data.x_printdata.pretty) { printf("X : \n"); state->x.pretty(stdout);}
		if(data.x_printdata.elems) { printf("X elements : \n"); state->x.printElems();}
		if(data.dx_printdata.pretty) { printf("dX : \n"); state->dx.pretty(stdout);}
		if(data.dx_printdata.elems) { printf("dX elements : \n"); state->dx.printElems();}
		if(data.ddx_printdata.pretty) { printf("ddX : \n"); state->ddx.pretty(stdout);}
		if(data.ddx_printdata.elems) { printf("ddX elements : \n"); state->ddx.printElems();}
	}

	else {
		ofstream stream;
		stream.open( data.x_printdata.file.toAscii());

		if(data.x_printdata.pretty) { stream <<  "X : \n" ; state->x.pretty(stream); }
		if(data.x_printdata.elems) { stream << "X elements : \n"; state->x.printElems(stream);}
		if(data.dx_printdata.pretty) {stream <<  "dX : \n"; state->dx.pretty(stream);}
		if(data.dx_printdata.elems) {stream << "dX elements : \n"; state->dx.printElems(stream);}
		if(data.ddx_printdata.pretty) { stream << "ddX : \n"; state->ddx.pretty(stream);}
		if(data.ddx_printdata.elems) { stream << "ddX elements : \n"; state->ddx.printElems(stream);}

		stream.close();
	}
	if(mutex != NULL) mutex->unlock();
}

void state_Idx_Gui::keyPressEvent( QKeyEvent* event){
	if(event->key() == Qt::Key_Equal){
		data.x_plotdata.zoom++;
		data.dx_plotdata.zoom++;
		data.ddx_plotdata.zoom++;
		refreshdisplay();
	}
	if(event->key() == Qt::Key_Minus){
		int validate;
		if(data.x_plotdata.zoom == 1) validate = QMessageBox::warning(this, "properties checking", "The zoom will be set to 0. Are you sure ?", QMessageBox::Yes | QMessageBox::No);
		else validate = QMessageBox::Yes;
		if(validate == QMessageBox::Yes){
			data.x_plotdata.zoom--;
			data.dx_plotdata.zoom--;
			data.ddx_plotdata.zoom--;
			refreshdisplay();
		}
	}
	if(event->key() == Qt::Key_P){
		print();
	}
	if(event->key() == Qt::Key_Q){
		close();
	}
}

void state_Idx_Gui::setvmin(double xmin, double dxmin, double ddxmin){
	data.x_plotdata.min = xmin;
	data.x_plotdata.norm = false;
	data.dx_plotdata.min = dxmin;
	data.dx_plotdata.norm = false;
	data.ddx_plotdata.min = ddxmin;
	data.ddx_plotdata.norm = false;
	refreshdisplay();
}

void state_Idx_Gui::setvmax(double xmax, double dxmax, double ddxmax){
	data.x_plotdata.max = xmax;
	data.x_plotdata.norm = false;
	data.dx_plotdata.max = dxmax;
	data.dx_plotdata.norm = false;
	data.ddx_plotdata.max = ddxmax;
	data.ddx_plotdata.norm = false;
	refreshdisplay();
}

void state_Idx_Gui::setvisible(bool xvisible, bool dxvisible, bool ddxvisible){
	data.x_plotdata.visible = xvisible;
	data.dx_plotdata.visible = dxvisible;
	data.ddx_plotdata.visible = ddxvisible;
	refreshdisplay();
}

void state_Idx_Gui::setidx1_as_array(bool xarray, bool dxarray, bool ddxarray){
	if( state->x.order() == 1){
		data.x_plotdata.idx1_as_array = xarray;
		data.dx_plotdata.idx1_as_array = dxarray;
		data.ddx_plotdata.idx1_as_array = ddxarray;
		refreshdisplay();
	}
}

void state_Idx_Gui::setidx3_as_RGB(bool xrgb, bool dxrgb, bool ddxrgb){
	if( state->x.order() >= 3){
		data.x_plotdata.idx3_as_RGB = xrgb;
		data.dx_plotdata.idx3_as_RGB = dxrgb;
		data.ddx_plotdata.idx3_as_RGB = ddxrgb;
		refreshdisplay();
	}
}

/******************************************************
//////////////////////////////////////////////////////////////////////////////////////////////
// specialized gui methods
//////////////////////////////////////////////////////////////////////////////////////////////
*****************************************************/

module_1_1_GUI::module_1_1_GUI(state_idx* in, state_idx* out, const char* title, QWidget *parent, QMutex* mutex):
	ebbox(parent, title, NULL, NULL, mutex),
	in(in),
	out(out)
	{
		add_box(new state_Idx_Gui(in, "in", this, mutex));
		add_box(new state_Idx_Gui(out, "out", this, mutex));
	}

module_1_1_GUI::module_1_1_GUI(const char* title, QWidget *parent, QMutex* mutex):
	ebbox(parent, title, NULL, NULL, mutex)
	{}

/* * * * * * * * * * * * * * * * * * * * * * */
module_2_1_GUI::module_2_1_GUI(state_idx* in1, state_idx* in2, state_idx* out, const char* title, QWidget *parent, QMutex* mutex):
	ebbox(parent, title, NULL, NULL, mutex),
	in1(in1),
	in2(in2),
	out(out)
	{
		add_box(new state_Idx_Gui(in1, "in1", this, mutex));
		add_box(new state_Idx_Gui(in2, "in2", this, mutex));
		add_box(new state_Idx_Gui(out, "out", this, mutex));
	}

module_2_1_GUI::module_2_1_GUI(const char* title, QWidget *parent, QMutex* mutex):
	ebbox(parent, title, NULL, NULL, mutex)
	{}

/* * * * * * * * * * * * * * * * * * * * * * */
layers_2_GUI::layers_2_GUI(layers_2<state_idx, state_idx, state_idx>* layer2, state_idx* in, state_idx* out, const char* title, QWidget *parent, QMutex* mutex):
	ebbox(parent, title, NULL, NULL, mutex),
	layer2(layer2),
	in(in),
	out(out)
	{
		add_box(new state_Idx_Gui(in, "in", this, mutex));
		add_box(new module_1_1_GUI("layer 1", this, mutex));
		add_box(new state_Idx_Gui(&layer2->hidden, "hidden", this, mutex));
		add_box(new module_1_1_GUI("layer 2", this, mutex));
		add_box(new state_Idx_Gui(out, "out", this, mutex));
	}

layers_2_GUI::layers_2_GUI(layers_2<state_idx, state_idx, state_idx>* layer2, const char* title, QWidget *parent, QMutex* mutex):
	ebbox(parent, title, NULL, NULL, mutex),
	layer2(layer2)
	{
		add_box(new module_1_1_GUI("layer 1", this, mutex));
		add_box(new state_Idx_Gui(&layer2->hidden, "hidden", this, mutex));
		add_box(new module_1_1_GUI("layer 2", this, mutex));
	}

/* * * * * * * * * * * * * * * * * * * * * * */
fc_ebm1_GUI::fc_ebm1_GUI(fc_ebm1<state_idx, state_idx>* ebm1, state_idx* in, state_idx* energy, const char* title, QWidget *parent, QMutex* mutex):
	ebbox(parent, title, NULL, NULL, mutex),
	ebm1(ebm1),
	in(in),
	energy(energy)
	{
		add_box(new state_Idx_Gui(in, "in", this, mutex));
		add_box(new module_1_1_GUI("layer 1", this, mutex));
		add_box(new state_Idx_Gui(&ebm1->fout, "out", this, mutex));
		add_box(new module_1_1_GUI("layer 2", this, mutex));
		add_box(new state_Idx_Gui(energy, "energy", this, mutex));
	}

fc_ebm1_GUI::fc_ebm1_GUI(fc_ebm1<state_idx, state_idx>* ebm1,const char* title, QWidget *parent, QMutex* mutex):
	ebbox(parent, title, NULL, NULL, mutex),
	ebm1(ebm1)
	{
		add_box(new module_1_1_GUI("layer 1", this, mutex));
		add_box(new state_Idx_Gui(&ebm1->fout, "out", this, mutex));
		add_box(new module_1_1_GUI("layer 2", this, mutex));
	}

/* * * * * * * * * * * * * * * * * * * * * * */
fc_ebm2_GUI::fc_ebm2_GUI(fc_ebm2<state_idx, state_idx, state_idx>* ebm2, state_idx* in1, state_idx* in2, state_idx* energy, const char* title, QWidget *parent, QMutex* mutex):
	ebbox(parent, title, NULL, NULL, mutex),
	ebm2(ebm2),
	in1(in1),
	in2(in2),
	energy(energy)
	{
		add_box(new state_Idx_Gui(in1, "in1", this, mutex));
		add_box(new state_Idx_Gui(in2, "in2", this, mutex));
		add_box(new module_1_1_GUI("layer 1", this, mutex));
		add_box(new state_Idx_Gui(&ebm2->fout, "out", this, mutex));
		add_box(new module_1_1_GUI("layer 2", this, mutex));
		add_box(new state_Idx_Gui(energy, "energy", this, mutex));
	}

fc_ebm2_GUI::fc_ebm2_GUI(fc_ebm2<state_idx, state_idx, state_idx>* ebm2, const char* title, QWidget *parent, QMutex* mutex):
	ebbox(parent, title, NULL, NULL, mutex),
	ebm2(ebm2)
	{
		add_box(new module_1_1_GUI("layer 1", this, mutex));
		add_box(new state_Idx_Gui(&ebm2->fout, "out", this, mutex));
		add_box(new module_1_1_GUI("layer 2", this, mutex));
	}

/* * * * * * * * * * * * * * * * * * * * * * */
linear_module_GUI::linear_module_GUI(linear_module* module, state_idx *in, state_idx *out, const char* title, QWidget *parent, QMutex* mutex):
	module_1_1_GUI(in, out, title, parent, mutex),
	module(module)
	{
		add_box(new state_Idx_Gui(&module->w, "w", this, mutex));
	}

linear_module_GUI::linear_module_GUI(linear_module* module, const char* title, QWidget *parent, QMutex* mutex):
	module_1_1_GUI(title, parent, mutex),
	module(module)
	{
		add_box(new state_Idx_Gui(&module->w, "w", this, mutex));
	}

/* * * * * * * * * * * * * * * * * * * * * * */
nn_layer_full_GUI::nn_layer_full_GUI(nn_layer_full* layer, state_idx *in, state_idx *out, const char* title, QWidget *parent, QMutex* mutex):
	module_1_1_GUI(in, out, title, parent, mutex),
	layer(layer)
	{
	  add_box(new linear_module_GUI(&layer->linear, 
					"linear module", this, mutex));
		add_box(new state_Idx_Gui(&layer->adder.bias, "bias", this, mutex));
		add_box(new state_Idx_Gui(layer->sum, "sum", this, mutex));
		add_box(new module_1_1_GUI("sigmoid", this, mutex));
	}

nn_layer_full_GUI::nn_layer_full_GUI(nn_layer_full* layer, const char* title, QWidget *parent, QMutex* mutex):
	module_1_1_GUI(title, parent, mutex),
	layer(layer)
	{
	  add_box(new linear_module_GUI(&layer->linear,
					"linear module", this, mutex));
		add_box(new state_Idx_Gui(&layer->adder.bias, "bias", this, mutex));
		add_box(new state_Idx_Gui(layer->sum, "sum", this, mutex));
		add_box(new module_1_1_GUI("sigmoid", this, mutex));
	}

/* * * * * * * * * * * * * * * * * * * * * * */
f_layer_GUI::f_layer_GUI(f_layer* layer, state_idx *in, state_idx *out, const char* title, const char* squash_title, QWidget *parent, QMutex* mutex):
	module_1_1_GUI(in, out, title, parent, mutex),
	layer(layer)
	{
		add_box(new state_Idx_Gui(layer->weight, "weight", this, mutex));
		add_box(new state_Idx_Gui(layer->bias, "bias", this, mutex));
		add_box(new state_Idx_Gui(layer->sum, "sum", this, mutex));
		add_box(new module_1_1_GUI(squash_title, this, mutex));
	}

f_layer_GUI::f_layer_GUI(f_layer* layer, const char* title, const char* squash_title, QWidget *parent, QMutex* mutex):
	module_1_1_GUI(title, parent, mutex),
	layer(layer)
	{
		add_box(new state_Idx_Gui(layer->weight, "weight", this, mutex));
		add_box(new state_Idx_Gui(layer->bias, "bias", this, mutex));
		add_box(new state_Idx_Gui(layer->sum, "sum", this, mutex));
		add_box(new module_1_1_GUI(squash_title, this, mutex));
	}

/* * * * * * * * * * * * * * * * * * * * * * */
c_layer_GUI::c_layer_GUI(c_layer* layer, state_idx *in, state_idx *out, const char* title, const char* squash_title, QWidget *parent, QMutex* mutex):
	module_1_1_GUI(in, out, title, parent, mutex),
	layer(layer)
	{
		add_box(new Idx_Gui((void*)(layer->table), INTG, "table", this, mutex));
		add_box(new state_Idx_Gui(layer->kernel, "kernel", this, mutex));
		add_box(new state_Idx_Gui(layer->bias, "bias", this, mutex));
		add_box(new state_Idx_Gui(layer->sum, "sum", this, mutex));
		add_box(new module_1_1_GUI(squash_title, this, mutex));
	}

c_layer_GUI::c_layer_GUI(c_layer* layer, const char* title, const char* squash_title, QWidget *parent, QMutex* mutex):
	module_1_1_GUI(title, parent, mutex),
	layer(layer)
	{
	add_box(new Idx_Gui((void*)(layer->table), INTG, "table", this, mutex));
		add_box(new state_Idx_Gui(layer->kernel, "kernel", this, mutex));
		add_box(new state_Idx_Gui(layer->bias, "bias", this, mutex));
		add_box(new state_Idx_Gui(layer->sum, "sum", this, mutex));
		add_box(new module_1_1_GUI(squash_title, this, mutex));
	}

/* * * * * * * * * * * * * * * * * * * * * * */
s_layer_GUI::s_layer_GUI(s_layer* layer, state_idx *in, state_idx *out, const char* title, const char* squash_title, QWidget *parent, QMutex* mutex):
	module_1_1_GUI(in, out, title, parent, mutex),
	layer(layer)
	{
		add_box(new state_Idx_Gui(layer->coeff, "coeff", this, mutex));
		add_box(new state_Idx_Gui(layer->bias, "bias", this, mutex));
		add_box(new state_Idx_Gui(layer->sub, "sub", this, mutex));
		add_box(new state_Idx_Gui(layer->sum, "sum", this, mutex));
		add_box(new module_1_1_GUI(squash_title, this, mutex));
	}

s_layer_GUI::s_layer_GUI(s_layer* layer, const char* title, const char* squash_title, QWidget *parent, QMutex* mutex):
	module_1_1_GUI(title, parent, mutex),
	layer(layer)
	{
		add_box(new state_Idx_Gui(layer->coeff, "coeff", this, mutex));
		add_box(new state_Idx_Gui(layer->bias, "bias", this, mutex));
		add_box(new state_Idx_Gui(layer->sub, "sub", this, mutex));
		add_box(new state_Idx_Gui(layer->sum, "sum", this, mutex));
		add_box(new module_1_1_GUI(squash_title, this, mutex));
	}

/* * * * * * * * * * * * * * * * * * * * * * */

logadd_layer_GUI::logadd_layer_GUI(logadd_layer* layer, state_idx *in, state_idx *out, const char* title, QWidget *parent, QMutex* mutex):
	module_1_1_GUI(in, out, title, parent, mutex),
	layer(layer)
	{
		add_box(new Idx_Gui((void*)&(layer->expdist), DOUBLE, "expdist", this, mutex));
		add_box(new Idx_Gui((void*)&(layer->sumexp), DOUBLE, "sumexp", this, mutex));
	}

logadd_layer_GUI::logadd_layer_GUI(logadd_layer* layer, const char* title, QWidget *parent, QMutex* mutex):
	module_1_1_GUI(title, parent, mutex),
	layer(layer)
	{
		add_box(new Idx_Gui((void*)&(layer->expdist), DOUBLE, "expdist", this, mutex));
		add_box(new Idx_Gui((void*)&(layer->sumexp), DOUBLE, "sumexp", this, mutex));
	}

/* * * * * * * * * * * * * * * * * * */

edist_cost_GUI::edist_cost_GUI(edist_cost* edist, state_idx *in, state_idx *energy, const char* title, QWidget *parent, QMutex* mutex):
	module_1_1_GUI(in, energy, title, parent, mutex),
	edist(edist)
	{
		add_box(new logadd_layer_GUI(edist->logadder, "log-add layer", this, mutex));
		add_box(new state_Idx_Gui(edist->dist, "dist", this, mutex));
		add_box(new state_Idx_Gui(edist->logadded_dist, "logadded_dist", this, mutex));
		add_box(new Idx_Gui((void*)(edist->proto), DOUBLE, "proto", this, mutex));
		add_box(new Idx_Gui((void*)&(edist->label2classindex), UBYTE, "proto", this, mutex));
	}

edist_cost_GUI::edist_cost_GUI(edist_cost* edist, const char* title, QWidget *parent, QMutex* mutex):
	module_1_1_GUI( title, parent, mutex),
	edist(edist)
	{
		add_box(new logadd_layer_GUI(edist->logadder, "log-add layer", this, mutex));
		add_box(new state_Idx_Gui(edist->dist, "dist", this, mutex));
		add_box(new state_Idx_Gui(edist->logadded_dist, "logadded_dist", this, mutex));
		add_box(new Idx_Gui((void*)(edist->proto), DOUBLE, "proto", this, mutex));
		add_box(new Idx_Gui((void*)&(edist->label2classindex), UBYTE, "proto", this, mutex));
	}

/* * * * * * * * * * * * * * * * * * * * * */

ebwindow::ebwindow(int height, int width):
	QWidget()
	{
	resize(width, height);
	mylabel = new QLabel(this);
    mylabel->setGeometry(QRect(0, 0, width, height));
    mylabel->setScaledContents(true);

    mydisplay = new QPixmap(width, height);
    QColor color;
    color.setRgb(255, 255, 255);
    mydisplay->fill(color.rgb());
    mylabel->setPixmap(*mydisplay);

    painter = new QPainter(mydisplay);

    show();
}

void ebwindow::resizeEvent (QResizeEvent *event){
	resize(event->size());
	mylabel->resize(event->size());
}

void ebwindow::gray_draw_matrix(void* idx, idx_type type, int x, int y, int minv, int maxv, int zoomx, int zoomy, QMutex* mutex){
	if(mutex != NULL) mutex->lock();
	int order = static_cast<Idx<double>*>(idx)->order();
	if(order < 2){
		eblerror("not designed for idx0 and idx1");
		if(mutex != NULL) mutex->unlock();
		return;
	}
	int height = (order == 2 )? static_cast<Idx<double>*>(idx)->dim(0): static_cast<Idx<double>*>(idx)->dim(1);
	int width = (order == 2 )? static_cast<Idx<double>*>(idx)->dim(1): static_cast<Idx<double>*>(idx)->dim(2);
	Idx<ubyte>* mycopy = new Idx<ubyte>(height, width);

	int min, max;
	if((minv == 0)&&(maxv == 0)){
		min = idx_min(*mycopy);
		max = idx_max(*mycopy);
	} else {
		min = minv;
		max = maxv;
	}

	if(type == DOUBLE){
		Idx<double> test1(height, width);
		if(order==2) idx_copy(*(static_cast<Idx<double>*>(idx)), test1);
		else {
			Idx<double> bla = static_cast<Idx<double>*>(idx)->select(0,0);
			idx_copy(bla, test1);
		}
		idx_dotc( test1, (double)(255/(max-min)), test1);
		idx_addc( test1, (double)(-min*255/(max-min)), test1);
		idx_copy( test1, *mycopy);
	}
	if(type == FLOAT){
		Idx<float> test2(height, width);
		if(order==2) idx_copy(*(static_cast<Idx<float>*>(idx)), test2);
		else {
			Idx<float> bla = static_cast<Idx<float>*>(idx)->select(0,0);
			idx_copy(bla, test2);
		}
		idx_dotc( test2, (float)(255/(max-min)), test2);
		idx_addc( test2, (float)(-min*255/(max-min)), test2);
		idx_copy( test2, *mycopy);
	}
	if(type == INTG){
		if(order==2) idx_copy(*(static_cast<Idx<intg>*>(idx)), *mycopy);
		else {
			Idx<intg> test3 = static_cast<Idx<intg>*>(idx)->select(2,0);
			idx_copy(test3, *mycopy);
		}
		idx_dotc(*mycopy, (ubyte)(255/(max-min)), *mycopy);
		idx_addc(*mycopy, (ubyte)(-min*255/(max-min)), *mycopy);
	}
	if(type == UBYTE){
		if(order==2) idx_copy(*(static_cast<Idx<ubyte>*>(idx)), *mycopy);
		else {
			Idx<ubyte> test4 = static_cast<Idx<ubyte>*>(idx)->select(2,0);
			idx_copy(test4, *mycopy);
		}
		idx_dotc(*mycopy, (ubyte)(255/(max-min)), *mycopy);
		idx_addc(*mycopy, (ubyte)(-min*255/(max-min)), *mycopy);
	}
	if(mutex != NULL) mutex->unlock();

	QImage* image;
	QVector<QRgb> table(256);
	for (int i = 0; i < 256; i++){
	    table[i] = qRgb(i, i, i);
	}
	image = new QImage((uchar*)mycopy->idx_ptr(), width, height, width * sizeof(unsigned char), QImage::Format_Indexed8);
	image->setColorTable(table);
	image = new QImage(image->scaled(width*zoomx, height*zoomy));
	painter->drawImage(x, y, *image);

	mylabel->setPixmap(*mydisplay);
}

void ebwindow::RGB_draw_matrix(void* idx, idx_type type, int x, int y, int zoomx, int zoomy, QMutex* mutex){
	if(mutex != NULL) mutex->lock();
	int order = static_cast<Idx<double>*>(idx)->order();
	if(order != 3){
		eblerror("designed for idx3 only !");
		if(mutex != NULL) mutex->unlock();
		return;
	}
	int width = static_cast<Idx<double>*>(idx)->dim(2);
	int height = static_cast<Idx<double>*>(idx)->dim(3);
	Idx<ubyte> myimage(height, width, 4);
	Idx<ubyte> mypartimage = myimage.narrow(2,3,0);
	if(type == DOUBLE){
		Idx<double> test1 = static_cast<Idx<double>*>(idx)->narrow(0,3,0);
		idx_copy(test1, mypartimage);
	}
	if(type == FLOAT){
		Idx<float> test2 = static_cast<Idx<float>*>(idx)->narrow(0,3,0);
		idx_copy(test2, mypartimage);
	}
	if(type == INTG){
		Idx<intg> test3 = static_cast<Idx<intg>*>(idx)->narrow(0,3,0);
		idx_copy(test3, mypartimage);
	}
	if(type == UBYTE){
		Idx<ubyte> test4 = static_cast<Idx<ubyte>*>(idx)->narrow(0,3,0);
		idx_copy(test4, mypartimage);
	}
	if(mutex != NULL) mutex->unlock();


	QImage* image;
	image = new QImage((uchar*)myimage.idx_ptr(), width, height, width * sizeof(unsigned char), QImage::Format_RGB32);
	image = new QImage(image->scaled(width*zoomx, height*zoomy).rgbSwapped());
	painter->drawImage(x, y, *image);

	mylabel->setPixmap(*mydisplay);
}

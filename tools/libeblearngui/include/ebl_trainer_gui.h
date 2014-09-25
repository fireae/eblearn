/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Redistribution under a license not approved by the Open Source
 *       Initiative (http://www.opensource.org) must display the
 *       following acknowledgement in all advertising material:
 *        This product includes software developed at the Courant
 *        Institute of Mathematical Sciences (http://cims.nyu.edu).
 *     * The names of the authors may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ThE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#ifndef EBL_TRAINER_GUI_H_
#define EBL_TRAINER_GUI_H_

#include "libidxgui.h"
#include "ebl_trainer.h"
#include "datasource_gui.h"
#include "ebl_arch_gui.h"

namespace ebl {

// supervised Trainer gui //////////////////////////////////////////////////////

template <class T, class Tdata, class Tlabel>
class supervised_trainer_gui : public scroll_box {
 public:
  //! add scrolling controls if scroll is true.
  supervised_trainer_gui(const char *title = NULL, bool scroll = false);
  virtual ~supervised_trainer_gui();

  //! displays nh x nw samples of the datasource ds 3 times:
  //! 1- with groundtruth labels
  //! 2- with classification answers, correct and wrong
  //! 3- samples with wrong answers only
  void display_datasource(supervised_trainer<T,Tdata,Tlabel> &st,
                          labeled_datasource<T,Tdata,Tlabel> &ds,
                          infer_param &infp,
                          unsigned int nh, unsigned int nw,
                          unsigned int h0 = 0, unsigned int w0 = 0,
                          double zoom = 1.0, int wid = -1,
                          const char *title = NULL, bool scrolling = false);

  //! displays internal states of the <ninternals> first samples of ds.
  void display_internals(supervised_trainer<T,Tdata,Tlabel> &st,
                         labeled_datasource<T,Tdata,Tlabel> &ds,
                         infer_param &infp, gd_param &args,
                         unsigned int ninternals,
                         unsigned int h0 = 0, unsigned int w0 = 0,
                         double zoom = 1.0, int wid = -1,
                         const char *title = NULL);

  //! displays correct or incorrect nh x nw samples of the datasource ds,
  //! ordered by decreasing energies.
  //! This method assumes correctness has been previously set in
  //! datasource by a test call.
  //! \param incorrect Show incorrect samples if true, correct otherwise.
  //! \param up If true, sort starting with lowest energies,
  //!        highest otherwise.
  void display_correctness(bool incorrect, bool up,
                           supervised_trainer<T,Tdata,Tlabel> &st,
                           labeled_datasource<T,Tdata,Tlabel> &ds,
                           infer_param &infp,
                           unsigned int nh, unsigned int nw,
                           bool print_raw_outputs = false,
                           bool print_energy = false,
                           bool draw_all_jitter = false,
                           bool show_only_images = false,
                           unsigned int h0 = 0, unsigned int w0 = 0,
                           double zoom = 1.0, int wid = -1,
                           const char *title = NULL, bool scrolling = false);

  //////////////////////////////////////////////////////////////////////////////
  // inherited methods to implement for scrolling capabilities

  //! scrolling method.
  virtual void display_next ();
  //! scrolling method.
  virtual void display_previous ();
  //! scrolling method.
  virtual unsigned int max_pages ();
  //    virtual scroll_box0* copy();
  //! deep copy (for independent multi-threaded display).
  virtual supervised_trainer_gui<T,Tdata,Tlabel>* copy();

  // members /////////////////////////////////////////////////////////////////
 protected:
  supervised_trainer<T,Tdata,Tlabel>     *_st;
  labeled_datasource<T,Tdata,Tlabel>     *_ds;
  labeled_datasource<T,Tdata,Tlabel>     *_last_ds;
  infer_param				 *_infp;
  unsigned int                            _nh;
  unsigned int                            _nw;
  double				  _zoom;
  int					  datasource_wid;
  int					  datasource_wid2;
  int					  datasource_wid3;
  int					  datasource_wid4;
  int					  internals_wid;
  int					  internals_wid2;
  int					  internals_wid3;
  bool                                    scroll;
  bool                                    scroll_added;
  unsigned int                            pos;
  labeled_datasource_gui<T,Tdata,Tlabel> *dsgui;
  string                                  title0; //!< Main title.
  string                                  title1;
  string                                  title2;
  string                                  title3;
  string                                  title4;
};

} // namespace ebl {

#include "ebl_trainer_gui.hpp"

#endif /* EBL_TRAINER_GUI_H_ */

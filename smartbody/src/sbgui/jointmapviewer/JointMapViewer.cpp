#include "vhcl.h"
#include "JointMapViewer.h"
#include "PositionControl.h"
#include <sb/SBJointMapManager.h>
#include <sb/SBJointMap.h>
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBSimulationManager.h>
#include <sk/sk_joint.h>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Input.H>
#include <sstream>
#include <cstring>
#include <FL/Fl.H>
#include <FL/gl.h>
#include <FL/glu.h>
#include <FL/glut.H>
#include <sr/sr_gl.h>
#include <sr/sr_gl_render_funcs.h>
#include <sr/sr_sphere.h>
#include <sr/sr_sn_shape.h>
#include <sbm/gwiz_math.h>
#include <boost/algorithm/string/replace.hpp>

#ifndef WIN32
#define _strdup strdup
#endif

# define ROTATING2(e)	(e.alt && e.button1)
# define DOLLYING(e)	(e.alt && e.button3)
# define TRANSLATING(e)	(e.alt && e.button2)

char commenSkString[] = "# SK Skeleton Definition - M. Kallmann 2004\n"
	"\n"	
	"\n"
	"skeleton\n"
	"root base\n"
	"{ offset 0 0 0\n"
	"  channel XPos 0 free\n"
	"  channel YPos 0 free\n"
	"  channel ZPos 0 free\n"
	"  channel Quat\n"
	"\n"
	"  joint spine1\n"
	"  { offset 0 7.782680 0\n"
	"    channel Quat\n"
	"\n"
	"    joint spine2\n"
	"    { offset 0 9.925360 0\n"
	"      channel Quat\n"
	"\n"
	"      joint spine3\n"
	"      { offset 0 11.171900 0\n"
	"        channel Quat\n"
	"\n"
	"        joint spine4\n"
	"        { offset 0 27.120060 0\n"
	"          channel Quat\n"
	"\n"
	"          joint spine5\n"
	"          { offset 0 3.711200 0\n"
	"            channel Quat\n"
	"\n"
	"            joint skullbase\n"
	"            { offset 0 5 0\n"
	"              channel Quat\n"
	"\n"
	"              joint face_top_parent\n"
	"              { offset -0.038970 3.909850 -1.942020\n"
	"                channel Quat\n"
	"\n"
	"                joint brow_parent_left\n"
	"                { offset 2.820000 3.780000 9.660000\n"
	"                  channel Quat\n"
	"\n"
	"                  joint brow01_left\n"
	"                  { offset -1.780870 -0.049450 3.780000\n"
	"                    channel XPos 0 lim -0.519130 0.320870\n"
	"                    channel YPos 0 lim -0.730550 1.059450\n"
	"                    channel Quat\n"
	"                  }\n"
	"\n"
	"                  joint brow02_left\n"
	"                  { offset -0.353630 0.564340 3.390000\n"
	"                    channel XPos 0 lim -0.436370 0.853630\n"
	"                    channel YPos 0 lim -0.734340 1.055660\n"
	"                    channel Quat\n"
	"                  }\n"
	"\n"
	"                  joint brow03_left\n"
	"                  { offset 1.590810 0.592430 2.690000\n"
	"                    channel XPos 0 lim -0.610810 0.239190\n"
	"                    channel YPos 0 lim -0.732430 1.057570\n"
	"                    channel Quat\n"
	"                  }\n"
	"\n"
	"                  joint brow04_left\n"
	"                  { offset 2.499990 -0.299720 1.960000\n"
	"                    channel XPos 0 lim -0.249990 0.500010\n"
	"                    channel YPos 0 lim -0.730280 1.059720\n"
	"                    channel Quat\n"
	"                  }\n"
	"                }\n"
	"\n"
	"                joint ear_left\n"
	"                { offset 6.637030 -1.991910 5.352970\n"
	"                  channel Quat\n"
	"                }\n"
	"\n"
	"                joint eyeball_left\n"
	"                { offset 2.945130 2.035140 10.222710\n"
	"                  channel Quat\n"
	"                }\n"
	"\n"
	"                joint upper_nose_left\n"
	"                { offset 1.404970 0.147520 12.040000\n"
	"                  channel XPos 0 lim -0.104970 0.095030\n"
	"                  channel YPos 0 lim -0.147520 0.462480\n"
	"                  channel Quat\n"
	"\n"
	"                  joint lower_nose_left\n"
	"                  { offset 0.455180 -2.310930 0.390000\n"
	"                   channel XPos 0 lim -0.455180 0.394820\n"
	"                   channel YPos 0 lim -0.289070 0.310930\n"
	"                    channel Quat\n"
	"                  }\n"
	"                }\n"
	"\n"
	"                joint lower_eyelid_left\n"
	"                { offset 3.150000 1.367480 11.570000\n"
	"                  channel YPos 0 lim -0.307480 0.232520\n"
	"                  channel Quat\n"
	"                }\n"
	"\n"
	"                joint upper_eyelid_left\n"
	"                { offset 3.150000 3.038480 11.750000\n"
	"                  channel YPos 0 lim -0.788480 0.361520\n"
	"                  channel Quat\n"
	"                }\n"
	"\n"
	"                joint brow_parent_right\n"
	"                { offset -2.820000 3.780000 9.660000\n"
	"                  channel Quat\n"
	"\n"
	"                  joint brow01_right\n"
	"                  { offset 1.780870 -0.049380 3.780000\n"
	"                    channel XPos 0 lim -0.320870 0.439130\n"
	"                    channel YPos 0 lim -0.850620 0.979380\n"
	"                    channel Quat\n"
	"                  }\n"
	"\n"
	"                  joint brow02_right\n"
	"                  { offset 0.353630 0.564490 3.390000\n"
	"                    channel XPos 0 lim -0.853630 0.436370\n"
	"                    channel YPos 0 lim -0.854490 0.975510\n"
	"                    channel Quat\n"
	"                  }\n"
	"\n"
	"                  joint brow03_right\n"
	"                  { offset -1.590750 0.658370 2.690000\n"
	"                    channel XPos 0 lim -0.239250 0.610750\n"
	"                    channel YPos 0 lim -0.848370 0.971630\n"
	"                    channel Quat\n"
	"                  }\n"
	"\n"
	"                  joint brow04_right\n"
	"                  { offset -2.499980 -0.299570 1.960000\n"
	"                    channel XPos 0 lim -0.500020 0.249980\n"
	"                    channel YPos 0 lim -0.850430 0.969570\n"
	"                    channel Quat\n"
	"                  }\n"
	"                }\n"
	"\n"
	"                joint upper_eyelid_right\n"
	"                { offset -3.080000 3.038340 11.750000\n"
	"                  channel YPos 0 lim -0.788340 0.371660\n"
	"                  channel Quat\n"
	"                }\n"
	"\n"
	"                joint eyeball_right\n"
	"                { offset -2.842170 2.036090 10.221480\n"
	"                  channel Quat\n"
	"                }\n"
	"\n"
	"                joint lower_eyelid_right\n"
	"                { offset -3.080000 1.367430 11.570000\n"
	"                  channel YPos 0 lim -0.307430 0.232570\n"
	"                  channel Quat\n"
	"                }\n"
	"\n"
	"                joint upper_nose_right\n"
	"                { offset -1.327030 0.147620 12.040000\n"
	"                  channel XPos 0 lim -0.212970 0.127030\n"
	"                  channel YPos 0 lim -0.147620 0.452380\n"
	"                  channel Quat\n"
	"\n"
	"                  joint lower_nose_right\n"
	"                  { offset -0.455180 -2.310990 0.390000\n"
	"                    channel XPos 0 lim -0.394820 0.385180\n"
	"                    channel YPos 0 lim -0.289010 0.310990\n"
	"                    channel Quat\n"
	"                  }\n"
	"                }\n"
	"\n"
	"                joint ear_right\n"
	"                { offset -6.559100 -1.991770 5.352970\n"
	"                  channel Quat\n"
	"                }\n"
	"              }\n"
	"\n"
	"              joint joint18\n"
	"              { offset 0 17.447189 0.002250\n"
	"                channel Quat\n"
	"              }\n"
	"\n"
	"              joint face_bottom_parent\n"
	"              { offset 0 0.145190 1.289570\n"
	"                channel Quat\n"
	"\n"
	"                joint Jaw\n"
	"                { offset -0.038970 -0.111010 2.057260\n"
	"                  channel Quat\n"
	"\n"
	"                  joint Jaw_back\n"
	"                  { offset 0 -0.913910 0.210480\n"
	"                    channel Quat\n"
	"\n"
	"                    joint Jaw_front\n"
	"                    { offset 0 -4.202680 6.052890\n"
	"                      channel Quat\n"
	"\n"
	"                      joint Lip_bttm_mid\n"
	"                      { offset -0.052560 2.885640 0.870980\n"
	"                        channel XPos 0 lim -0.597440 0.602560\n"
	"                        channel YPos 0 lim -0.945640 1.024360\n"
	"                        channel ZPos 0 lim -0.910980 0.019020\n"
	"                        channel Quat\n"
	"                      }\n"
	"\n"
	"                      joint Lip_bttm_left\n"
	"                      { offset 1.930000 3.449230 0.250950\n"
	"                        channel XPos 0 lim -0.500000 0.500000\n"
	"                        channel YPos 0 lim -1.289230 1.030770\n"
	"                        channel ZPos 0 lim -0.650950 0.359050\n"
	"                        channel Quat\n"
	"                      }\n"
	"\n"
	"                      joint Lip_bttm_right\n"
	"                      { offset -1.507840 3.433670 0.220870\n"
	"                        channel XPos 0 lim -0.922160 0.077840\n"
	"\t\t\t\t\t\tchannel YPos 0 lim -1.273670 1.046330\n"
	"                        channel ZPos 0 lim -0.620870 0.389130\n"
	"                        channel Quat\n"
	"                      }\n"
	"                    }\n"
	"                  }\n"
	"\n"
	"                  joint Tongue_back\n"
	"                  { offset 0 -1.510320 1.882950\n"
	"                    channel Quat\n"
	"\n"
	"                    joint Tongue_mid\n"
	"                    { offset 0.038970 0.680370 2.435230\n"
	"                      channel Quat\n"
	"\n"
	"                      joint Tongue_front\n"
	"                      { offset -0.038970 0.037470 1.683220\n"
	"                        channel Quat\n"
	"                      }\n"
	"                    }\n"
	"                  }\n"
	"                }\n"
	"\n"
	"                joint Lip_top_left\n"
	"                { offset 1.851790 0.085060 8.668930\n"
	"                  channel XPos 0 lim -0.741790 0.258210\n"
	"                  channel YPos 0 lim -0.695060 0.444940\n"
	"                  channel ZPos 0 lim -0.208930 0.791070\n"
	"                  channel Quat\n"
	"                }\n"
	"\n"
	"                joint cheek_low_left\n"
	"                { offset 4.580000 0.191310 6.440000\n"
	"                  channel YPos 0 lim -1.291310 2.128690\n"
	"                  channel Quat\n"
	"                }\n"
	"\n"
	"                joint Cheek_up_left\n"
	"                { offset 4.130000 4.254030 8.960000\n"
	"                  channel YPos 0 lim -1.024030 0.505970\n"
	"                  channel Quat\n"
	"                }\n"
	"\n"
	"                joint Lip_out_left\n"
	"                { offset 2.325820 -0.885000 8.113830\n"
	"                  channel XPos 0 lim -0.775820 0.634180\n"
	"                  channel YPos 0 lim -0.645000 0.475000\n"
	"                  channel ZPos 0 lim -0.903830 0.216170\n"
	"                  channel Quat\n"
	"                }\n"
	"\n"
	"                joint Lip_top_mid\n"
	"                { offset -0.020000 0.356390 9.562470\n"
	"                  channel YPos 0 lim -0.356390 1.263610\n"
	"                  channel ZPos 0 lim -0.252470 0.247530\n"
	"                  channel Quat\n"
	"                }\n"
	"\n"
	"                joint Cheek_up_right\n"
	"                { offset -4.130000 4.253970 9.010000\n"
	"                  channel YPos 0 lim -1.023970 0.506030\n"
	"                  channel Quat\n"
	"                }\n"
	"\n"
	"                joint cheek_low_right\n"
	"                { offset -4.570000 0.191290 6.630000\n"
	"                  channel YPos 0 lim -1.761290 1.498710\n"
	"                  channel Quat\n"
	"                }\n"
	"\n"
	"                joint Lip_top_right\n"
	"                { offset -1.740000 0.084910 8.668940\n"
	"                  channel XPos 0 lim -0.370000 0.630000\n"
	"                  channel YPos 0 lim -0.694910 0.445090\n"
	"                  channel ZPos 0 lim -0.208940 0.791060\n"
	"                  channel Quat\n"
	"                }\n"
	"\n"
	"                joint Lip_out_right\n"
	"                { offset -2.325820 -0.884920 8.113840\n"
	"                  channel XPos 0 lim -0.474180 0.615820\n"
	"                  channel YPos 0 lim -0.485080 0.574920\n"
	"                  channel ZPos 0 lim -0.823840 0.036160\n"
	"                  channel Quat\n"
	"                }\n"
	"              }\n"
	"            }\n"
	"          }\n"
	"        }\n"
	"\n"
	"        joint l_sternoclavicular\n"
	"        { offset 1 23.788891 5.732900\n"
	"          channel Quat\n"
	"\n"
	"          joint l_acromioclavicular\n"
	"          { offset 7 -5.668830 -5.732900\n"
	"            channel Quat\n"
	"\n"
	"            joint l_shoulder\n"
	"            { offset 8 4 0\n"
	"              channel Quat\n"
	"\n"
	"              joint l_elbow\n"
	"              { offset 28 0.430660 -0.030480\n"
	"                channel Quat\n"
	"\n"
	"                joint l_forearm\n"
	"                { offset 15.998100 0.246060 -0.017420\n"
	"                  channel Quat\n"
	"\n"
	"                  joint l_wrist\n"
	"                  { offset 11.998570 0.184540 -0.013060\n"
	"                    channel Quat\n"
	"\n"
	"                    joint l_pinky1\n"
	"                    { offset 6.580830 -0.692350 -3.343890\n"
	"                      channel Quat\n"
	"\n"
	"                      joint l_pinky2\n"
	"                      { offset 2.415330 0.037150 -0.002630\n"
	"                        channel Quat\n"
	"\n"
	"                        joint l_pinky3\n"
	"                        { offset 2.475000 0.038070 0.027130\n"
	"                          channel Quat\n"
	"\n"
	"                          joint l_pinky4\n"
	"                          { offset 2.355630 0.036230 -0.062210\n"
	"                            channel Quat\n"
	"                          }\n"
	"                        }\n"
	"                      }\n"
	"                    }\n"
	"\n"
	"                    joint l_ring1\n"
	"                    { offset 7.509180 -0.313580 -1.525730\n"
	"                      channel Quat\n"
	"\n"
	"                      joint l_ring2\n"
	"                      { offset 3.757020 0.057780 -0.153200\n"
	"                        channel Quat\n"
	"\n"
	"                        joint l_ring3\n"
	"                        { offset 3.190560 0.049070 -0.063120\n"
	"                          channel Quat\n"
	"\n"
	"                          joint l_ring4\n"
	"                          { offset 2.653850 0.040820 -0.032710\n"
	"                            channel Quat\n"
	"                          }\n"
	"                        }\n"
	"                      }\n"
	"                    }\n"
	"\n"
	"                    joint l_middle1\n"
	"                    { offset 8.219700 -0.013310 -0.008950\n"
	"                      channel Quat\n"
	"\n"
	"                      joint l_middle2\n"
	"                      { offset 3.999520 0.061510 -0.004350\n"
	"                        channel Quat\n"
	"\n"
	"                        joint l_middle3\n"
	"                        { offset 2.999640 0.046140 -0.003270\n"
	"                          channel Quat\n"
	"\n"
	"                          joint l_middle4\n"
	"                          { offset 2.999640 0.046140 -0.003270\n"
	"                            channel Quat\n"
	"                          }\n"
	"                        }\n"
	"                      }\n"
	"                    }\n"
	"\n"
	"                    joint l_index1\n"
	"                    { offset 7.761800 -0.167860 1.963990\n"
	"                      channel Quat\n"
	"\n"
	"                      joint l_index2\n"
	"                      { offset 4.204470 0.064670 -0.004580\n"
	"                        channel Quat\n"
	"\n"
	"                        joint l_index3\n"
	"                        { offset 3.071320 0.047240 -0.033170\n"
	"                          channel Quat\n"
	"\n"
	"                          joint l_index4\n"
	"                          { offset 2.185490 0.033610 0.055010\n"
	"                            channel Quat\n"
	"                          }\n"
	"                        }\n"
	"                      }\n"
	"                    }\n"
	"\n"
	"                    joint l_thumb1\n"
	"                    { offset 2.405290 -0.229900 3.521100\n"
	"                      channel Quat\n"
	"\n"
	"                      joint l_thumb2\n"
	"                      { offset 0.003380 0.000050 3.105480\n"
	"                        channel Quat\n"
	"\n"
	"                        joint l_thumb3\n"
	"                        { offset 0.002610 0.000040 2.401110\n"
	"                          channel Quat\n"
	"\n"
	"                          joint l_thumb4\n"
	"                          { offset 0.002380 0.000040 2.182370\n"
	"                            channel Quat\n"
	"                          }\n"
	"                        }\n"
	"                      }\n"
	"                    }\n"
	"                  }\n"
	"                }\n"
	"              }\n"
	"            }\n"
	"          }\n"
	"        }\n"
	"\n"
	"        joint r_sternoclavicular\n"
	"        { offset -1 23.788891 5.732890\n"
	"          channel Quat\n"
	"\n"
	"          joint r_acromioclavicular\n"
	"          { offset -6.999990 -5.668980 -5.732890\n"
	"            channel Quat\n"
	"\n"
	"            joint r_shoulder\n"
	"            { offset -7.999990 4.000200 0\n"
	"              channel Quat\n"
	"\n"
	"              joint r_elbow\n"
	"              { offset -28.000031 0.430680 -0.030480\n"
	"                channel Quat\n"
	"\n"
	"                joint r_forearm\n"
	"                { offset -15.998040 0.245970 -0.017420\n"
	"                  channel Quat\n"
	"\n"
	"                  joint r_wrist\n"
	"                  { offset -11.998760 0.184400 -0.013060\n"
	"                    channel Quat                    \n"
	"                    \n"
	"                    joint r_pinky1\n"
	"                    { offset -6.580630 -0.692200 -3.343900\n"
	"                      channel Quat\n"
	"\n"
	"                      joint r_pinky2\n"
	"                      { offset -2.415540 0.037190 -0.002620\n"
	"                        channel Quat\n"
	"\n"
	"                        joint r_pinky3\n"
	"                        { offset -2.474980 0.038100 0.027130\n"
	"                          channel Quat\n"
	"\n"
	"                          joint r_pinky4\n"
	"                          { offset -2.355490 0.036270 -0.062210\n"
	"                            channel Quat\n"
	"                          }\n"
	"                        }\n"
	"                      }\n"
	"                    }\n"
	"\n"
	"                    joint r_ring1\n"
	"                    { offset -7.509050 -0.313330 -1.525730\n"
	"                      channel Quat\n"
	"\n"
	"                      joint r_ring2\n"
	"                      { offset -3.756960 0.057610 -0.153200\n"
	"                        channel Quat\n"
	"\n"
	"                        joint r_ring3\n"
	"                        { offset -3.190650 0.049070 -0.063120\n"
	"                          channel Quat\n"
	"\n"
	"                          joint r_ring4\n"
	"                          { offset -2.653890 0.040840 -0.032710\n"
	"                            channel Quat\n"
	"                          }\n"
	"                        }\n"
	"                      }\n"
	"                    }\n"
	"\n"
	"                    joint r_middle1\n"
	"                    { offset -8.219540 -0.013110 -0.008950\n"
	"                      channel Quat\n"
	"\n"
	"                      joint r_middle2\n"
	"                      { offset -3.999590 0.061570 -0.004350\n"
	"                        channel Quat\n"
	"\n"
	"                        joint r_middle3\n"
	"                        { offset -2.999540 0.046020 -0.003270\n"
	"                          channel Quat\n"
	"\n"
	"                          joint r_middle4\n"
	"                          { offset -2.999840 0.046020 -0.003270\n"
	"                            channel Quat\n"
	"                          }\n"
	"                        }\n"
	"                      }\n"
	"                    }\n"
	"\n"
	"                    joint r_index1\n"
	"                    { offset -7.761730 -0.167640 1.963990\n"
	"                      channel Quat\n"
	"\n"
	"                      joint r_index2\n"
	"                      { offset -4.204410 0.064620 -0.004580\n"
	"                        channel Quat\n"
	"\n"
	"                        joint r_index3\n"
	"                        { offset -3.071160 0.047240 -0.033170\n"
	"                          channel Quat\n"
	"\n"
	"                          joint r_index4\n"
	"                          { offset -2.185720 0.033530 0.055010\n"
	"                            channel Quat\n"
	"                          }\n"
	"                        }\n"
	"                      }\n"
	"                    }\n"
	"\n"
	"                    joint r_thumb1\n"
	"                    { offset -2.405180 -0.229820 3.521110\n"
	"                      channel Quat\n"
	"\n"
	"                      joint r_thumb2\n"
	"                      { offset -0.003350 0 3.105490\n"
	"                        channel Quat\n"
	"\n"
	"                        joint r_thumb3\n"
	"                        { offset -0.002740 0.000300 2.401090\n"
	"                          channel Quat\n"
	"\n"
	"                          joint r_thumb4\n"
	"                          { offset -0.002130 0 2.182370\n"
	"                            channel Quat\n"
	"                          }\n"
	"                        }\n"
	"                      }\n"
	"                    }\n"
	"                  }\n"
	"                }\n"
	"              }\n"
	"            }\n"
	"          }\n"
	"        }\n"
	"      }\n"
	"    }\n"
	"  }\n"
	"\n"
	"  joint l_hip\n"
	"  { offset 10 -5 0\n"
	"    channel Quat\n"
	"\n"
	"    joint l_knee\n"
	"    { offset 0 -43.999020 0.293680\n"
	"      channel Quat\n"
	"\n"
	"      joint l_ankle\n"
	"      { offset 0 -43.999020 0.293680\n"
	"        channel Quat\n"
	"\n"
	"        joint l_forefoot\n"
	"        { offset 0 -6.879700 18.046320\n"
	"          channel Quat\n"
	"\n"
	"          joint l_toe\n"
	"          { offset 0 0.046720 6.999840\n"
	"            channel Quat\n"
	"          }\n"
	"        }\n"
	"      }\n"
	"    }\n"
	"  }\n"
	"\n"
	"  joint r_hip\n"
	"  { offset -10 -5 0\n"
	"    channel Quat\n"
	"\n"
	"    joint r_knee\n"
	"    { offset 0 -43.999039 0.293680\n"
	"      channel Quat\n"
	"\n"
	"      joint r_ankle\n"
	"      { offset 0 -43.999100 0.293680\n"
	"        channel Quat\n"
	"\n"
	"        joint r_forefoot\n"
	"        { offset 0 -6.879640 18.046320\n"
	"          channel Quat\n"
	"\n"
	"          joint r_toe\n"
	"          { offset 0 0.046630 6.999850\n"
	"            channel Quat\n"
	"          }\n"
	"        }\n"
	"      }\n"
	"    }\n"
	"  }\n"
	"}\n"
	"\n"
	"end\n"
	"";

MouseViewer::MouseViewer( int x, int y, int w, int h, char* name ) : Fl_Gl_Window(x,y,w,h,name)
{

}

MouseViewer::~MouseViewer()
{

}

void MouseViewer::preRender()
{
	glViewport ( 0, 0, w(), h() );		
	SrMat mat ( SrMat::NotInitialized );
	// 	static int counter = 0;
	// 	counter = counter%1000;
	// 	float color = ((float)counter)/1000.f;
	// 	counter++;
	glClearColor ( SrColor(0.8f,0.8f,0.8f));
	//glClearColor ( SrColor(1.0f,1.0f,1.0f));
	//glClearColor(SrColor(color,color,color));
	//----- Clear Background --------------------------------------------	
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable ( GL_LIGHTING );
	for (size_t x = 0; x < lights.size(); x++)
	{
		glLight ( x, lights[x] );		
	}

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.2f);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.08f);	

	static GLfloat mat_emissin[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_ambient[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_diffuse[] = { 1.0,  1.0,    1.0,    1.0 };
	static GLfloat mat_speclar[] = { 0.0,  0.0,    0.0,    1.0 };
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, mat_emissin );
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_speclar );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0 );
	glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );
	//glEnable( GL_COLOR_MATERIAL );
	glEnable( GL_NORMALIZE );

	//----- Set Projection ----------------------------------------------
	cam.setAspectRatio((float)w()/(float)h());

	glMatrixMode ( GL_PROJECTION );
	glLoadMatrix ( cam.get_perspective_mat(mat) );

	//----- Set Visualisation -------------------------------------------
	glMatrixMode ( GL_MODELVIEW );
	glLoadMatrix ( cam.get_view_mat(mat) );

	glScalef ( cam.getScale(), cam.getScale(), cam.getScale() );
}

void MouseViewer::init_opengl()
{
	// valid() is turned on by fltk after draw() returns
	glViewport ( 0, 0, w(), h() );
	glEnable ( GL_DEPTH_TEST );
	glEnable ( GL_LIGHT0 ); 
	glEnable ( GL_LIGHTING );

	//glEnable ( GL_BLEND ); // for transparency
	//glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glCullFace ( GL_BACK );
	glDepthFunc ( GL_LEQUAL );
	glFrontFace ( GL_CCW );

	glEnable ( GL_POLYGON_SMOOTH );

	//glEnable ( GL_LINE_SMOOTH );
	//glHint ( GL_LINE_SMOOTH_HINT, GL_NICEST );

	glEnable ( GL_POINT_SMOOTH );
	glPointSize ( 2.0 );

	glShadeModel ( GL_SMOOTH );		
}

void MouseViewer::updateLights()
{
	SrLight light;		
	light.directional = true;
	light.diffuse = SrColor( 1.0f, 1.0f, 1.0f );
	light.position = SrVec( 100.0, 250.0, 400.0 );
	//	light.constant_attenuation = 1.0f/cam.scale;
	light.constant_attenuation = 1.0f;
	lights.push_back(light);

	SrLight light2 = light;
	light2.directional = true;
	light2.diffuse = SrColor( 0.8f, 0.8f, 0.8f );
	light2.position = SrVec( 100.0, 500.0, -1000.0 );
	//	light2.constant_attenuation = 1.0f;
	//	light2.linear_attenuation = 2.0f;
	lights.push_back(light2);
}


int MouseViewer::handle( int event )
{
	switch ( event )
	{ 
	case FL_PUSH:
		translate_event ( e, SrEvent::EventPush, w(), h(), this );
		break;

	case FL_RELEASE:
		translate_event ( e, SrEvent::EventRelease, w(), h(), this);
		break;

	case FL_DRAG:
		translate_event ( e, SrEvent::EventDrag, w(), h(), this );
		break;

	case FL_MOVE:

		break;

	case FL_WHEN_RELEASE:
		//translate_event ( e, SrEvent::EventRelease, w(), h(), this);
		break;

	case FL_KEYBOARD:
		break;

	case FL_HIDE: // Called when the window is iconized
		break;

	case FL_SHOW: // Called when the window is de-iconized or when show() is called
		show ();
		break;

	default:
		break;
	}

	mouse_event(e);

	if (event == FL_PUSH)
		return 1;

	return Fl_Gl_Window::handle(event);
}


void MouseViewer::translate_event( SrEvent& e, SrEvent::EventType t, int w, int h, MouseViewer* viewer )
{
	e.init_lmouse ();

	// put coordinates inside [-1,1] with (0,0) in the middle :
	e.mouse.x  = ((float)Fl::event_x())*2.0f / ((float)w) - 1.0f;
	e.mouse.y  = ((float)Fl::event_y())*2.0f / ((float)h) - 1.0f;
	e.mouse.y *= -1.0f;
	e.width = w;
	e.height = h;
	e.mouseCoord.x = (float)Fl::event_x();
	e.mouseCoord.y = (float)Fl::event_y();

	if ( Fl::event_state(FL_BUTTON1) ) 
		e.button1 = 1;

	if ( Fl::event_state(FL_BUTTON2) ) 
		e.button2 = 1;

	if ( Fl::event_state(FL_BUTTON3) ) 
		e.button3 = 1;


	if(e.button1 == 0 && e.button2 == 0 && e.button3 == 0) 
	{
		t = SrEvent::EventRelease;
	}

	e.type = t;

	if ( t==SrEvent::EventPush)
	{
		e.button = Fl::event_button();
		e.origUp = viewer->cam.getUpVector();
		e.origEye = viewer->cam.getEye();
		e.origCenter = viewer->cam.getCenter();
		e.origMouse.x = e.mouseCoord.x;
		e.origMouse.y = e.mouseCoord.y;
	}
	else if (t==SrEvent::EventRelease )
	{
		e.button = Fl::event_button();
		e.origMouse.x = -1;
		e.origMouse.y = -1;
	}


	if ( Fl::event_state(FL_ALT)   ) e.alt = 1;
	else e.alt = 0;
	if ( Fl::event_state(FL_CTRL)  ) e.ctrl = 1;
	else e.ctrl = 0;
	if ( Fl::event_state(FL_SHIFT) ) e.shift = 1;
	else e.shift = 0;

	e.key = Fl::event_key();	
}

void MouseViewer::mouse_event( SrEvent& e )
{
	if ( e.type==SrEvent::EventDrag )
	{ 
		float dx = e.mousedx() * cam.getAspectRatio();
		float dy = e.mousedy() / cam.getAspectRatio();
		if ( DOLLYING(e) )
		{ 
			float amount = dx;
			SrVec cameraPos(cam.getEye());
			SrVec targetPos(cam.getCenter());
			SrVec diff = targetPos - cameraPos;
			float distance = diff.len();
			diff.normalize();

			if (amount >= distance)
				amount = distance - .000001f;

			SrVec diffVector = diff;
			SrVec adjustment = diffVector * distance * amount;
			cameraPos += adjustment;
			SrVec oldEyePos = cam.getEye();
			cam.setEye(cameraPos.x, cameraPos.y, cameraPos.z);
			SrVec cameraDiff = cam.getEye() - oldEyePos;
			SrVec center = cam.getCenter();
			center += cameraDiff;
			cam.setCenter(center.x, center.y, center.z);
			redraw();
		}
		else if ( TRANSLATING(e) )
		{ 
			cam.apply_translation_from_mouse_motion ( e.lmouse.x, e.lmouse.y, e.mouse.x, e.mouse.y );
			redraw();
		}
		else if ( ROTATING2(e) )
		{ 
			float deltaX = -(e.mouseCoord.x - e.origMouse.x) / e.width;
			float deltaY = -(e.mouseCoord.y -  e.origMouse.y) / e.height;
			if (deltaX == 0.0 && deltaY == 0.0)
				return;

			SrVec origUp = e.origUp;
			SrVec origCenter = e.origCenter;
			SrVec origCamera = e.origEye;

			SrVec dirX = origUp;
			SrVec  dirY;
			dirY.cross(origUp, (origCenter - origCamera));
			dirY /= dirY.len();

			SrVec camera = rotate_point(origCamera, origCenter, dirX, -deltaX * float(M_PI));
			camera = rotate_point(camera, origCenter, dirY, deltaY * float(M_PI));

			cam.setEye(camera.x, camera.y, camera.z);
			redraw();
		}
	}
}

SrVec MouseViewer::rotate_point(SrVec point, SrVec origin, SrVec direction, float angle)
{
	float originalLength = point.len();

	SrVec v = direction;
	SrVec o = origin;
	SrVec p = point;
	float c = cos(angle);
	float s = sin(angle);
	float C = 1.0f - c;

	SrMat mat;
	mat.e11() = v[0] * v[0] * C + c;
	mat.e12() = v[0] * v[1] * C - v[2] * s;
	mat.e13() = v[0] * v[2] * C + v[1] * s;
	mat.e21() = v[1] * v[0] * C + v[2] * s;
	mat.e22() = v[1] * v[1] * C + c;
	mat.e23() = v[1] * v[2] * C - v[0] * s;
	mat.e31() = v[2] * v[0] * C - v[1] * s;
	mat.e32() = v[2] * v[1] * C + v[0] * s;
	mat.e33() = v[2] * v[2] * C + c;

	mat.transpose();

	SrVec result = origin + mat * (point - origin);

	return result;
}


/************************************************************************/
/* Skeleton Viewer                                                      */
/************************************************************************/

SkeletonViewer::SkeletonViewer( int x, int y, int w, int h, char* name ) : MouseViewer(x,y,w,h,name)
{
	skeletonScene = NULL;
	skeleton = NULL;
	jointMapName = "";
	focusJointName = "";
	showJointLabels = 0;
	curTime = 0.f;
	prevTime = 0.f;
	motionTime = 0.f;
	testMotion = NULL;
	playMotion = false;
}

SkeletonViewer::~SkeletonViewer()
{

}

void SkeletonViewer::setTestMotion( SmartBody::SBMotion* motion )
{	
	testMotion = motion;
	if (testMotion)
		testMotion->connect(skeleton);
}

void SkeletonViewer::setPlayMotion( bool play )
{
	// replay motion
	if (play && !playMotion)
	{
		// reset motion time
		motionTime = 0.f;
	}
	else // clear skeleton
	{
		skeleton->clearJointValues();	
		skeletonScene->update();
	}
	playMotion = play;
}

int SkeletonViewer::handle( int event )
{
	bool pickMsg = false;
	if (event == FL_PUSH)
	{
		translate_event ( e, SrEvent::EventPush, w(), h(), this );
		if (e.button1)
		{
			make_current();
			std::string focusName = pickJointName(e.mouse.x,e.mouse.y);
			//LOG("focus joint name = %s",focusName.c_str());
			if (focusName != "")
			{
				if (focusName != focusJointName)
				{
					setFocusJointName(focusName);
				}
				else
				{
					setFocusJointName("");

				}				
				pickMsg = true;
			}
		}
	}
	if (!pickMsg)
		return MouseViewer::handle(event);
	else
		return Fl_Gl_Window::handle(event);
}

std::string SkeletonViewer::pickJointName( float x, float y )
{
	if (!skeleton) return "";

	GLint viewport[4];
	const int BUFSIZE = 512;
	GLuint selectBuf[BUFSIZE];
	float ratio;	
	SrMat mat;

	glSelectBuffer(BUFSIZE,selectBuf);
	glGetIntegerv(GL_VIEWPORT,viewport);
	glRenderMode(GL_SELECT);
	glInitNames();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	int cursorX,cursorY;
	cursorX = int((x+1.0)*viewport[2]*0.5);
	cursorY = int((y+1.0)*viewport[3]*0.5);

	//gluPickMatrix(cursorX,viewport[3]-cursorY,100,100,viewport);
	//printf("cursorX = %d, cursorY = %d\n",cursorX,cursorY);
	gluPickMatrix(cursorX,cursorY,5,5,viewport);
	ratio = (viewport[2]+0.0f) / viewport[3];	
	glMultMatrixf ( (const float*)cam.get_perspective_mat(mat) );	
	glMatrixMode(GL_MODELVIEW);

	// draw buffer object for each joints ?	
	// test for each joints
	//pawn_control.hitTest();
	
	float jointSize = 0.5;	
	jointSize = skeleton->getCurrentHeight() / 50.0f;
	for (int i=0;i<skeleton->getNumJoints();i++)
	{
		SmartBody::SBJoint* joint = skeleton->getJoint(i);
		SrVec jointPos = joint->gmat().get_translation();
		glPushName(0xffffffff);
		glLoadName(i);
		SrVec color(1,0,0);
		PositionControl::drawSphere(jointPos, jointSize, color);								
		glPopName();			
	}	
	// process picking
	GLint hits;
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	hits = glRenderMode(GL_RENDER);
	std::vector<int> hitNames;	
	std::string jointName = "";
	if (hits != 0){
		//processHits2(hits,selectBuf,0);
		hitNames = process_hit(selectBuf,hits);		
		int jointIdx = hitNames[0];
		if (jointIdx >= 0 && jointIdx < skeleton->getNumJoints())
		{
			jointName = skeleton->getJoint(jointIdx)->getName();
		}
	}	
	return jointName;	
}


void SkeletonViewer::setShowJointLabels( int showLabel )
{
	showJointLabels = showLabel;
}

std::vector<int> SkeletonViewer::process_hit(unsigned int *pickbuffer,int nhits)
{
	GLuint d1,d2,i,n,zmin,zmax,sel=0;
	std::vector<int> hitNames;
	if(0<=nhits){    
		for(i=0,zmin=zmax=4294967295U; nhits>0; i+=n+3,nhits--)
		{      
			n=pickbuffer[i];
			d1=pickbuffer[1+i];
			d2=pickbuffer[2+i];
			if(d1<zmin || (d1==zmin && d2<=zmax)){
				zmin=d1;
				zmax=d2;
				sel=i;
			}
		}
		//return pickbuffer[3+sel];
		int n = pickbuffer[sel];
		for (int k=0;k<n;k++)
			hitNames.push_back(pickbuffer[3+sel+k]);
	}
	return hitNames;
}

void SkeletonViewer::setJointMap( std::string mapName )
{
	jointMapName = mapName;
}


void SkeletonViewer::setFocusJointName( std::string focusName )
{
	focusJointName = focusName;
	if (skeleton)
	{
		float defaultRadius, defaultLen;
		skeletonScene->get_defaults(defaultRadius,defaultLen);
		//skeletonScene->set_skeleton_radius(defaultRadius*10.f);

		std::vector<std::string> skelJointNames = skeleton->getJointNames();
		std::vector<std::string> skelOriginalNames = skeleton->getJointOriginalNames();
		for (unsigned int i=0;i<skelJointNames.size();i++)
		{
			std::string jname = skelJointNames[i];
			std::string origName = skelOriginalNames[i];
			SkJoint* joint = skeleton->search_joint(jname.c_str());
			if (focusJointName == jname || focusJointName == origName)
			{
				//LOG("focus joint name = %s",jname.c_str());
				skeletonScene->setJointColor(joint, SrColor(1.f,0.f,0.f));
				skeletonScene->setJointRadius(joint, defaultRadius*2.5f);
			}
			else
			{
				//skeletonScene->setJointColor(joint, SrColor(1.f,1.f,1.f));
				skeletonScene->setJointColor(joint, SrColor(1.f,1.f,1.f));
				skeletonScene->setJointRadius(joint, defaultRadius);
			}
		}
	}	
}


std::string SkeletonViewer::getFocusJointName()
{
	return focusJointName;
}



void SkeletonViewer::setSkeleton( std::string skelName )
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBSkeleton* sk = scene->getSkeleton(skelName);
	if (sk)
	{
		skeleton = sk;
	}

	bool success = updateSkeleton();
	if (success)
		focusOnSkeleton();
}

bool SkeletonViewer::updateSkeleton()
{

	if (skeletonScene)
	{
		delete skeletonScene;
		skeletonScene = NULL;
	}
	if (!skeleton) return false;		
	skeletonScene = new SkScene();
	skeletonScene->ref();
	skeletonScene->init(skeleton);
	skeletonScene->set_visibility(true,false,false,false);
	return true;
}




void SkeletonViewer::focusOnSkeleton()
{
	SrBox sceneBox;
	if (!skeleton) return;

	sceneBox = skeleton->getBoundingBox();	
	cam.view_all(sceneBox, cam.getFov());	
	float scale = skeleton->getCurrentHeight();
	float znear = 0.01f*scale;
	float zfar = 100.0f*scale;
	cam.setNearPlane(znear);
	cam.setFarPlane(zfar);

}

void SkeletonViewer::draw()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBSimulationManager* simManager = scene->getSimulationManager();	

	prevTime = curTime;
	curTime = (float)simManager->getTime();
	deltaTime = curTime - prevTime;
	
	if (playMotion && testMotion)
	{
		motionTime += deltaTime;
		// normalize motion time
		while (motionTime > testMotion->getDuration())
		{
			motionTime -= (float)testMotion->getDuration();
		}
		testMotion->apply(motionTime);
		for (int i=0;i<3;i++) // fix the root position
			skeleton->root()->pos()->value(i,0.f);
		skeletonScene->update();
	}

	make_current();

	if (!visible()) 
		return;
	
	if (!valid()) 
	{
		init_opengl();
		valid(1);
	}

	glViewport ( 0, 0, w(), h() );		
	SrMat mat ( SrMat::NotInitialized );
// 	static int counter = 0;
// 	counter = counter%1000;
// 	float color = ((float)counter)/1000.f;
// 	counter++;
	//glClearColor ( SrColor(0.5f,0.5f,0.5f));
	glClearColor ( SrColor(1.0f,1.0f,1.0f));
	//glClearColor(SrColor(color,color,color));
	//----- Clear Background --------------------------------------------	
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable ( GL_LIGHTING );
	for (size_t x = 0; x < lights.size(); x++)
	{
		glLight ( x, lights[x] );		
	}

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.2f);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.08f);	

	static GLfloat mat_emissin[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_ambient[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_diffuse[] = { 1.0,  1.0,    1.0,    1.0 };
	static GLfloat mat_speclar[] = { 0.0,  0.0,    0.0,    1.0 };
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, mat_emissin );
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_speclar );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0 );
	glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );
	//glEnable( GL_COLOR_MATERIAL );
	glEnable( GL_NORMALIZE );

	//----- Set Projection ----------------------------------------------
	cam.setAspectRatio((float)w()/(float)h());

	glMatrixMode ( GL_PROJECTION );
	glLoadMatrix ( cam.get_perspective_mat(mat) );

	//----- Set Visualisation -------------------------------------------
	glMatrixMode ( GL_MODELVIEW );
	glLoadMatrix ( cam.get_view_mat(mat) );

	glScalef ( cam.getScale(), cam.getScale(), cam.getScale() );
	if (skeletonScene)
	{	
		renderFunction.apply(skeletonScene);	
		drawJointMapLabels(jointMapName);
	}	
}

void SkeletonViewer::drawJointMapLabels( std::string jointMapName )
{
	//glPushAttrib(GL_LIGHTING | GL_COLOR_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	float sceneScale = scene->getScale();
	if (sceneScale == 0.0f)
		sceneScale = 1.0f;
	float textSize = skeleton->getCurrentHeight()*0.0002f;
	
	SmartBody::SBJointMapManager* jointMapManager = scene->getJointMapManager();
	SmartBody::SBJointMap* jointMap = jointMapManager->getJointMap(jointMapName);		
	std::vector<std::string> skelJointNames = skeleton->getJointNames();
	std::vector<std::string> skelOrigNames = skeleton->getJointOriginalNames();
	

	for (unsigned int j = 0; j < skelJointNames.size(); j++)
	{
		bool highLight = false;
		std::string jointName = skelJointNames[j];
		std::string origName = skelOrigNames[j];
		//if (jointMap->getMapTarget(jointName) == "")
		//	continue;
		if (jointName == focusJointName || origName == focusJointName)
			highLight = true;

		if (!highLight && showJointLabels == 0)
			continue;

		std::string source = origName;
		if (showJointLabels == 2)
			source = jointName;

		SkJoint* joint = skeleton->search_joint(source.c_str());
		if (!joint) continue;		

		const SrMat& mat = joint->gmat();
		glPushMatrix();
		glMultMatrixf((const float*) mat);
		float modelview[16];
		glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
		// undo all rotations
		// beware all scaling is lost as well 
		for( int a=0; a<3; a++ ) 
		{
			for( int b=0; b<3; b++ ) {
				if ( a==b )
					modelview[a*4+b] = 1.0;
				else
					modelview[a*4+b] = 0.0;
			}
		}
		// set the modelview with no rotations
		glLoadMatrixf(modelview);		
		if (highLight)
		{			
			glColor3f(1.0f, 0.0f, 0.0f);
			glScalef(textSize*1.5f, textSize*1.5f, textSize*1.5f);							
		}
		else
		{
			glColor3f(1.0f, 1.0f, 0.0f);
			glScalef(textSize, textSize, textSize);			
		}		
		glutStrokeString(GLUT_STROKE_ROMAN, (const unsigned char*) source.c_str());
		glPopMatrix();
		glColor3f(1.0f,1.0f,1.0f);
		glEnable(GL_DEPTH_TEST);
	}
	//glPopAttrib();	
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

SmartBody::SBSkeleton* SkeletonViewer::getSkeleton()
{
	return skeleton;
}


const std::string spineJointNames[7] = {"base","spine1", "spine2", "spine3", "spine4", "spine5", "skullbase" };
const std::string leftArmJointNames[5] = {"l_sternoclavicular","l_shoulder", "l_elbow", "l_forearm", "l_wrist" };
const std::string rightArmJointNames[5] = {"r_sternoclavicular","r_shoulder", "r_elbow", "r_forearm", "r_wrist" };
const std::string leftHandJointNames[20] = {"l_thumb1","l_thumb2", "l_thumb3", "l_thumb4", "l_index1", "l_index2","l_index3","l_index4","l_middle1", "l_middle2", "l_middle3", "l_middle4", "l_ring1", "l_ring2", "l_ring3","l_ring4", "l_pinky1", "l_pinky2", "l_pinky3","l_pinky4"};
const std::string rightHandJointNames[20] = {"r_thumb1","r_thumb2", "r_thumb3", "r_thumb4", "r_index1", "r_index2","r_index3","r_index4","r_middle1", "r_middle2", "r_middle3", "r_middle4", "r_ring1", "r_ring2", "r_ring3","r_ring4","r_pinky1", "r_pinky2", "r_pinky3","r_pinky4"};

const std::string leftLegJointNames[5] = { "l_hip", "l_knee", "l_ankle", "l_forefoot", "l_toe" };
const std::string rightLegJointNames[5] = { "r_hip", "r_knee", "r_ankle", "r_forefoot", "r_toe" };

JointMapViewer::JointMapViewer(int x, int y, int w, int h, char* name) : Fl_Double_Window(x, y, w, h, name)
{
	rootWindow = NULL;
	
	begin();	
	int curY = 10;
	int startY = 10;
	_charName ="";
	_jointMapName = "";
	_skelName = "";

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

	Fl_Group* leftGroup	= new Fl_Group(10, startY, 400, h - startY, "");
	leftGroup->begin();
// 	_choiceCharacters = new Fl_Choice(110, curY, 150, 20, "Character");
// 	//choiceCharacters->callback(CharacterCB, this);
// 	_choiceCharacters->callback(SelectCharacterCB,this);
// 
// 	updateCharacterList();
// 
// 	curY += 25;

	_choiceJointMaps = new Fl_Choice(110, curY, 150, 20, "JointMaps");
	//choiceCharacters->callback(CharacterCB, this);
	//std::vector<std::string> characters = scene->getCharacterNames();
	updateJointMapList();
	_choiceJointMaps->value(0);

	_choiceJointMaps->callback(SelectMapCB,this);

	curY += 35;	

	_buttonReset = new Fl_Button(20, curY, 80, 20, "Reset Map");
	_buttonReset->callback(ResetMapCB,this);

	_buttonRestore = new Fl_Button(120, curY, 80, 20, "Restore Map");
	_buttonRestore->callback(ResetMapCB,this);

	_buttonAutoMap = new Fl_Button(220, curY, 80, 20, "Auto Map");
	_buttonAutoMap->callback(ResetMapCB,this);



// 
// 
// 	SmartBody::SBBehaviorSetManager* behavMgr = SmartBody::SBScene::getScene()->getBehaviorSetManager();
// 	std::map<std::string, SmartBody::SBBehaviorSet*>& behavSets = behavMgr->getBehaviorSets();
// 
	
	for (int i=0;i<7;i++) standardJointNames.push_back(spineJointNames[i]);
	for (int i=0;i<5;i++) standardJointNames.push_back(leftArmJointNames[i]);
	for (int i=0;i<5;i++) standardJointNames.push_back(rightArmJointNames[i]);
	for (int i=0;i<20;i++) standardJointNames.push_back(leftHandJointNames[i]);
	for (int i=0;i<20;i++) standardJointNames.push_back(rightHandJointNames[i]);
	for (int i=0;i<5;i++) standardJointNames.push_back(leftLegJointNames[i]);
	for (int i=0;i<5;i++) standardJointNames.push_back(rightLegJointNames[i]);

	curY += 35;
	
 	_scrollGroup = new Fl_Scroll(10, curY, 400, h - curY - 50, "");
 	_scrollGroup->type(Fl_Scroll::VERTICAL);
 	_scrollGroup->begin(); 
	scrollY = curY;
	
	
	_scrollGroup->end();
	curY += (h-curY-50)  + 15;
	_buttonApply = new Fl_Button(40, curY, 120, 20, "Apply Map");
	_buttonApply->callback(ApplyMapCB, this);
	_buttonCancel = new Fl_Button(180, curY, 120, 20, "Cancel");
	_buttonCancel->callback(CancelCB, this);
	
	leftGroup->end();

	std::string commonSkName = "common.sk";	
	SmartBody::SBSkeleton* commonSk = scene->getSkeleton(commonSkName);
	if (!commonSk)
	{
		commonSk = scene->addSkeletonDefinition(commonSkName);
		commonSk->loadFromString(commenSkString);
		commonSk->ref();
	}

#if 0
	std::string commonSkMotionName = "commonSkTest";
	std::string motionExt = ".skm";
	SmartBody::SBMotion* commonSkMotion = scene->getMotion(commonSkMotionName);
	if (!commonSkMotion) // load from string
	{		
		std::string mediaPath = scene->getMediaPath();		
		std::string retargetDir = mediaPath + "/" + "behaviorsets/test/";
		std::string targetMotionFile = retargetDir+"/"+commonSkMotionName + motionExt;
		scene->loadAsset(targetMotionFile); // load motion		
	}
#endif

	Fl_Group* rightGroup = new Fl_Group(420, startY, w-420 , h - startY, "");
	rightGroup->begin();	
	targetSkeletonViewer = new SkeletonViewer(420+10, startY, 260, h-startY - 50, "Skeleton");	
	standardSkeletonViewer = new SkeletonViewer(420+20+260, startY, w-420-260, h-startY - 50, "Common Skeleton");
	_buttonJointLabel = new Fl_Choice(420 + 10 + 50, h - 50 + 10, 90, 30, "Joint Labels");
	_buttonJointLabel->add("None");
	_buttonJointLabel->add("Original Name");
	_buttonJointLabel->add("Mapped Name");
	_buttonJointLabel->value(0);
	_buttonJointLabel->callback(CheckShowJointLabelCB,this);
	_buttonAddMapping = new Fl_Button(420 + 10 + 150, h - 50 + 10, 90, 30, "Add Mapping");
	_buttonAddMapping->callback(AddJointMapCB,this);	
	_choiceTestMotions = new Fl_Choice(420 + 10 + 350, h - 50 + 10, 90, 30, "Test Motions");
	_choiceTestMotions->callback(ChangeTestMotionCB,this);
	_buttonTestPlayMotion = new Fl_Button(420 + 10 + 450, h - 50 + 10, 90, 30, "Run Test");
	_buttonTestPlayMotion->callback(TestPlayMotionCB,this);
	testPlay = false;
	rightGroup->end();
	//rightGroup->resizable(targetSkeletonViewer);		
	this->resizable(rightGroup);
	end();

// 	int numChildren = _scrollGroup->children();
// 	for (int i=0;i<numChildren;i++)
// 	{
// 		JointMapInputChoice* input = dynamic_cast<JointMapInputChoice*>(_scrollGroup->child(i));
// 		if (input)
// 			input->setViewer(skeletonViewer);
// 	}

	//testCommonSkMotion = scene->getMotion(commonSkMotionName);
	testTargetMotion = NULL;
	standardSkeletonViewer->setSkeleton(commonSkName);
	//standardSkeletonViewer->setTestMotion(testCommonSkMotion);	

	updateTestMotions();

	if (_choiceTestMotions->size() >= 1)
	{
		_choiceTestMotions->value(0);
		setTestMotion(_choiceTestMotions->text());
	}	



	SmartBody::SBJointMapManager* jointMapManager = scene->getJointMapManager();
	std::vector<std::string> jointMapNames = jointMapManager->getJointMapNames();
	if (_choiceJointMaps->size() > 0) 
	{
		_choiceJointMaps->value(0);
		setJointMapName(_choiceJointMaps->text());
		//updateSelectMap();
	}		
	_charName = "";
	_skelName = "";
}


void JointMapViewer::updateJointLists()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBSkeleton* skel = scene->getSkeleton(_skelName);
	int tempY = scrollY;
	
	std::vector<std::string> skelJointNames;
	if (skel)
	{
		for (unsigned int i=0;i<skel->joints().size();i++)
		{
			SmartBody::SBJoint* joint = skel->getJoint(i);
			skelJointNames.push_back(joint->extName());
		}
		//skelJointNames = skel->getJointNames();
	}
	for (unsigned int i=0;i<_jointChoiceList.size();i++)
	{
		JointMapInputChoice* input = _jointChoiceList[i];
		delete input;
	}
	_jointChoiceList.clear();
	_scrollGroup->clear();
	_scrollGroup->begin();
	for (unsigned int i=0;i<skelJointNames.size();i++)
	{
		std::string name = skelJointNames[i];
		//LOG("joint name = %s",name.c_str());
		//Fl_Check_Button* check = new Fl_Check_Button(20, curY, 100, 20, _strdup(name.c_str()));
		//Fl_Group* jointMapGroup = new Fl_Group(20, curY , 200, 20, _strdup(name.c_str()));
		//Fl_Input* input = new Fl_Input(100 , scrollY, 150, 20, _strdup(name.c_str()));
		JointMapInputChoice* choice = new JointMapInputChoice(140, tempY, 190, 20, _strdup(name.c_str()));
		_jointChoiceList.push_back(choice);		
		choice->input()->when(FL_WHEN_CHANGED);
		choice->input()->callback(JointNameChange,this);
		choice->menubutton()->when(FL_WHEN_CHANGED);
		choice->menubutton()->callback(JointNameChange,this);
		choice->setViewer(targetSkeletonViewer);
		tempY += 25;
	}
	_scrollGroup->end();
}


JointMapViewer::~JointMapViewer()
{
	if (testTargetMotion)
	{
		testTargetMotion->disconnect();
		delete testTargetMotion;
	}
}

void JointMapViewer::setShowButton(bool showButton)
{
	if (showButton)
	{
		_buttonApply->show();
		_buttonCancel->show();
	}
	else
	{
		_buttonApply->hide();
		_buttonCancel->hide();
	}	
}

void JointMapViewer::setCharacterName( std::string charName )
{
	_charName = charName;
// 	for (int c = 0; c < _choiceCharacters->size(); c++)
// 	{
// 		if (charName == _choiceCharacters->text(c))
// 		{
// 			_choiceCharacters->value(c);
// 			break;
// 		}
// 	}
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBCharacter* sbChar = scene->getCharacter(charName);
	if (sbChar && targetSkeletonViewer)
	{
		targetSkeletonViewer->setSkeleton(sbChar->getSkeleton()->getName());
		_skelName = sbChar->getSkeleton()->getName();
	}
	else // if there is no character or skeleton
	{
		targetSkeletonViewer->setSkeleton("");
	}

	updateJointLists();
	updateCharacter();	
	updateSelectMap();
}

void JointMapViewer::setJointMapName( std::string jointMapName )
{	
	_jointMapName = jointMapName;
	for (int c = 0; c < _choiceJointMaps->size(); c++)
	{
		if (jointMapName == _choiceJointMaps->text(c))
		{
			_choiceJointMaps->value(c);			
			break;
		}
	}

	updateCharacter();	
	updateSelectMap();	
	if (targetSkeletonViewer)
	{
		targetSkeletonViewer->setJointMap(jointMapName);
		targetSkeletonViewer->standardJointNames = standardJointNames;
		standardSkeletonViewer->standardJointNames = standardJointNames;
	}
}

void JointMapViewer::JointNameChange( Fl_Widget* widget, void* data )
{
	Fl_Input* input = dynamic_cast<Fl_Input*>(widget);
	Fl_Menu_Button* menuButton = dynamic_cast<Fl_Menu_Button*>(widget);

	Fl_Input_Choice* inputChoice = NULL;
	if (input)
		inputChoice = dynamic_cast<JointMapInputChoice*>(input->parent());
	if (menuButton)
		inputChoice = dynamic_cast<JointMapInputChoice*>(menuButton->parent());
	if (inputChoice)
	{
		JointMapViewer* viewer = (JointMapViewer*) data;
		viewer->updateJointName(inputChoice);
	}	
}


void JointMapViewer::updateJointName( Fl_Input_Choice* jointChoice )
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::string charName = _charName;
	SmartBody::SBCharacter* curChar = scene->getCharacter(charName);
	SmartBody::SBSkeleton* charSk = curChar->getSkeleton();		
	int valueIndex = jointChoice->menubutton()->value();	
	std::string choiceStr = "";
	if (valueIndex >= 0)
	{
		jointChoice->value(valueIndex);
		choiceStr = jointChoice->value();
		if (choiceStr == "--empty--")
			jointChoice->value("");
	}
	jointChoice->clear();
	jointChoice->add("--empty--"); // add empty string as the first choice
	std::string filterLabel = jointChoice->value();
	for (unsigned int i=0;i<standardJointNames.size();i++)
	{
		std::string& jname = standardJointNames[i];
		if (jname.find(filterLabel) != std::string::npos)
		{
			jointChoice->add(jname.c_str());			
		}
	}		
	if (valueIndex >= 0)
	{
		if (choiceStr == "--empty--")
			jointChoice->value("");
		else
			jointChoice->value(choiceStr.c_str());		
	}		
}

void JointMapViewer::updateCharacter()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::string charName = _charName;
	SmartBody::SBCharacter* curChar = scene->getCharacter(charName);
	if (!curChar) return;

	SmartBody::SBSkeleton* charSk = curChar->getSkeleton();
	skeletonJointNames = charSk->getJointNames();

	int numChildren = _scrollGroup->children();
	for (int i=0;i<numChildren;i++)
	{
		JointMapInputChoice* input = dynamic_cast<JointMapInputChoice*>(_scrollGroup->child(i));
		if (!input)
		{
			continue;
		}			
		updateJointName(input);	
	}
}


void JointMapViewer::autoGuessJointMap()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::string jointMapName = _skelName + "-autoMap";
	SmartBody::SBJointMapManager* jointMapManager = scene->getJointMapManager();
	SmartBody::SBJointMap* jointMap = jointMapManager->getJointMap(jointMapName);
	SmartBody::SBSkeleton* skel = scene->getSkeleton(_skelName);
	if (!skel) // no skeleton exists
		return;
	if (!jointMap && skel)
	{
		jointMap = jointMapManager->createJointMap(jointMapName);
		jointMap->guessMapping(scene->getSkeleton(_skelName), false);		
	}
	_jointMapName = jointMapName;
	updateJointMapList();
	setJointMapName(_jointMapName);
}

void JointMapViewer::updateSelectMap()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::string jointMapName = _jointMapName;
	SmartBody::SBJointMapManager* jointMapManager = scene->getJointMapManager();
	SmartBody::SBJointMap* jointMap = jointMapManager->getJointMap(jointMapName);
	if (!jointMap)  // create the guess mapping instead
	{	
		//autoGuessJointMap();
		return;	
	}

	int numChildren = _scrollGroup->children();
	for (int i=0;i<numChildren;i++)
	{
		Fl_Input_Choice* input = dynamic_cast<JointMapInputChoice*>(_scrollGroup->child(i));
		if (input)
		{
			std::string sourceName = input->label();
			const std::string& targetName = jointMap->getMapTarget(sourceName);
			if (targetName == "")
			{
				input->value("");
			}
			else
			{
				input->value(targetName.c_str());
			}
		}
	}
	
}


void JointMapViewer::applyJointMap()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::string jointMapName = _jointMapName;
	SmartBody::SBJointMapManager* jointMapManager = scene->getJointMapManager();
	SmartBody::SBJointMap* jointMap = jointMapManager->getJointMap(jointMapName);

	SmartBody::SBCharacter* curChar = scene->getCharacter(_charName);
	if (!curChar || !jointMap) return;
	int numChildren = _scrollGroup->children();
	jointMap->clearMapping();
	for (int i=0;i<numChildren;i++)
	{
		Fl_Input_Choice* input = dynamic_cast<JointMapInputChoice*>(_scrollGroup->child(i));
		if (input)
		{
			std::string sourceName = input->label();
			std::string targetName = input->value();	
			if (sourceName != "" && targetName != "")
				jointMap->setMapping(sourceName, targetName);
		}
	}	
	SmartBody::SBSkeleton* sceneSk = scene->getSkeleton(curChar->getSkeleton()->getName());
	jointMap->applySkeleton(sceneSk);
	jointMap->applySkeleton(curChar->getSkeleton());
	
	// in addition to update the skeleton, we also need to update the character controllers so all joint names are mapped correctly.
	//curChar->ct_tree_p->child_channels_updated(NULL);
}

void JointMapViewer::SelectMapCB( Fl_Widget* widget, void* data )
{
	Fl_Choice* mapChoice = dynamic_cast<Fl_Choice*>(widget);	
	JointMapViewer* viewer = (JointMapViewer*) data;
	viewer->setJointMapName(mapChoice->text());
	//viewer->updateSelectMap();
	//viewer->updateCharacter();	
}

void JointMapViewer::SaveMapCB( Fl_Widget* widget, void* data )
{
	Fl_Choice* mapChoice = dynamic_cast<Fl_Choice*>(widget);	
	JointMapViewer* viewer = (JointMapViewer*) data;
	viewer->setJointMapName(mapChoice->text());
	//viewer->updateSelectMap();
	//viewer->updateCharacter();	
}


// void JointMapViewer::SelectCharacterCB( Fl_Widget* widget, void* data )
// {
// 	JointMapViewer* viewer = (JointMapViewer*) data;	
// 	Fl_Choice* charChoice = dynamic_cast<Fl_Choice*>(widget);	
// 	viewer->setCharacterName(charChoice->text());
// 	//viewer->updateCharacter();	
// }

void JointMapViewer::AddJointMapCB( Fl_Widget* widget, void* data )
{
	JointMapViewer* viewer = (JointMapViewer*) data;	
	viewer->addFocusJointMap();
}

void JointMapViewer::ApplyMapCB(Fl_Widget* widget, void* data)
{
	JointMapViewer* viewer = (JointMapViewer*) data;
	viewer->applyJointMap();
	viewer->updateCharacter();

// 	SmartBody::SBBehaviorSetManager* behavMgr = SmartBody::SBScene::getScene()->getBehaviorSetManager();
// 
// 	// run the script associated with any checked behavior sets
// 	int numChildren = viewer->_scrollGroup->children();
// 	for (int c = 0; c < numChildren; c++)
// 	{
// 		Fl_Check_Button* check = dynamic_cast<Fl_Check_Button*>(viewer->_scrollGroup->child(c));
// 		if (check)
// 		{
// 			if (check->value())
// 			{
// 				SmartBody::SBBehaviorSet* behavSet = behavMgr->getBehaviorSet(check->label());
// 				if (behavSet)
// 				{
// 					LOG("Retargetting %s...", check->label());
// 					const std::string& script = behavSet->getScript();
// 					SmartBody::SBScene::getScene()->runScript(script.c_str());
// 					std::stringstream strstr;
// 					strstr << "setupBehaviorSet()";
// 					SmartBody::SBScene::getScene()->run(strstr.str());
// 					std::stringstream strstr2;
// 					strstr2 << "retargetBehaviorSet('" << viewer->getCharacterName() << "', '" << viewer->getSkeletonName() << "')";
// 					SmartBody::SBScene::getScene()->run(strstr2.str());
// 				}
// 			}
// 		}
// 	}
	//viewer->hide();
	if (viewer->rootWindow)
	{
		viewer->rootWindow->hide();
	}
}

void JointMapViewer::CancelCB(Fl_Widget* widget, void* data)
{
	JointMapViewer* viewer = (JointMapViewer*) data;
	//viewer->hide();
	if (viewer->rootWindow)
	{
		viewer->rootWindow->hide();
	}
}

void JointMapViewer::ResetMapCB( Fl_Widget* widget, void* data )
{
	JointMapViewer* viewer = (JointMapViewer*) data;
	Fl_Button* button = (Fl_Button*)widget;
	if (button == viewer->_buttonReset)
		viewer->resetJointMap(false);
	else if (button == viewer->_buttonRestore)
		viewer->resetJointMap(true);
	else if (button == viewer->_buttonAutoMap)
		viewer->autoGuessJointMap();
}

void JointMapViewer::CheckShowJointLabelCB( Fl_Widget* widget, void* data )
{
	JointMapViewer* viewer = (JointMapViewer*) data;
	Fl_Choice* checkButton = (Fl_Choice*) widget;		
	viewer->showJointLabels(checkButton->value());
}


void JointMapViewer::TestPlayMotionCB( Fl_Widget* widget, void* data )
{
	JointMapViewer* viewer = (JointMapViewer*) data;
	viewer->testPlayMotion();
}


void JointMapViewer::ChangeTestMotionCB( Fl_Widget* widget, void* data )
{
	JointMapViewer* viewer = (JointMapViewer*) data;
	Fl_Choice* motionChoice = (Fl_Choice*)widget;
	viewer->setTestMotion(motionChoice->text());	
}




void JointMapViewer::draw()
{
	if (targetSkeletonViewer)
		targetSkeletonViewer->redraw();
	if (standardSkeletonViewer)
		standardSkeletonViewer->redraw();
// 	int numChildren = _scrollGroup->children();
// 	for (int i=0;i<numChildren;i++)
// 	{
// 		Fl_Input_Choice* input = dynamic_cast<Fl_Input_Choice*>(_scrollGroup->child(i));
// 		
// 	}
	Fl_Double_Window::draw();
}

void JointMapViewer::addFocusJointMap()
{
	std::string standardJointName = standardSkeletonViewer->getFocusJointName();
	std::string targetJointName = targetSkeletonViewer->getFocusJointName();
	if (targetJointName == "" || standardJointName == "")
		return;

	int numChildren = _scrollGroup->children();

	for (int i=0;i<numChildren;i++)
	{
		Fl_Input_Choice* input = dynamic_cast<JointMapInputChoice*>(_scrollGroup->child(i));
		if (input)
		{
			std::string targetName = input->label();
			std::string standardName = input->value();
			if (standardName == standardJointName)
			{
				input->value("");
			}
			if (targetName == targetJointName)
			{
				input->value(standardJointName.c_str());
			}
		}
	}
}

void JointMapViewer::resetJointMap(bool restore)
{
	int numChildren = _scrollGroup->children();
	if (restore) 
	{
		updateSelectMap();
		return;
	}
	for (int i=0;i<numChildren;i++)
	{
		Fl_Input_Choice* input = dynamic_cast<JointMapInputChoice*>(_scrollGroup->child(i));
		if (input)
		{
			input->value("");
		}
	}	
}

void JointMapViewer::showJointLabels( int showLabel )
{
	LOG("joint label = %d",showLabel);
	standardSkeletonViewer->setShowJointLabels(showLabel);
	targetSkeletonViewer->setShowJointLabels(showLabel);
}

void JointMapViewer::testPlayMotion()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBSkeleton* targetSk = targetSkeletonViewer->getSkeleton();
	SmartBody::SBSkeleton* commonSk = standardSkeletonViewer->getSkeleton();
	if (!targetSk || !commonSk) return;

	if (!testCommonSkMotion) return;

	testPlay = !testPlay;
	if (testPlay)
	{
		_buttonTestPlayMotion->label("Stop Playing");	
	}
	else
	{
		_buttonTestPlayMotion->label("Test Motion");
	}
	standardSkeletonViewer->setPlayMotion(testPlay);

	
	// create a temporary retarget motion
	if (testPlay)
	{
		SmartBody::SBJointMap jointMap;
		int numChildren = _scrollGroup->children();
		for (int i=0;i<numChildren;i++)
		{
			Fl_Input_Choice* input = dynamic_cast<JointMapInputChoice*>(_scrollGroup->child(i));
			if (!input) continue;
			std::string mapValue = input->value();
			std::string mapSource = input->label();
			if (input && mapValue != "")
			{
				jointMap.setMapping(mapSource,mapValue);			
			}
		}
		std::vector<std::string> endJoints;
		std::vector<std::string> relativeJoints;
		std::map<std::string, SrVec> offsetJoints;	

		endJoints.push_back("l_forefoot");
		endJoints.push_back("l_toe");
		endJoints.push_back("l_acromioclavicular");
		endJoints.push_back("r_forefoot");	
		endJoints.push_back("r_toe");
		endJoints.push_back("r_acromioclavicular");
		endJoints.push_back("l_sternoclavicular");
		endJoints.push_back("r_sternoclavicular");

		relativeJoints.push_back("spine1");
		relativeJoints.push_back("spine2");
		relativeJoints.push_back("spine3");
		relativeJoints.push_back("spine4");
		relativeJoints.push_back("spine5");

		jointMap.applySkeleton(targetSk);	
		SmartBody::SBMotion* retargetMotion = dynamic_cast<SmartBody::SBMotion*>(testCommonSkMotion->buildRetargetMotionV2(commonSk,targetSk,endJoints,relativeJoints,offsetJoints));
		jointMap.applySkeletonInverse(targetSk);
		jointMap.applyMotionInverse(retargetMotion);
		if (testTargetMotion)
		{
			testTargetMotion->disconnect();
			delete testTargetMotion;
		}
		testTargetMotion = retargetMotion;
		targetSkeletonViewer->setTestMotion(testTargetMotion);
		standardSkeletonViewer->setTestMotion(testCommonSkMotion);
	}
	
	targetSkeletonViewer->setPlayMotion(testPlay);
}

void JointMapViewer::updateTestMotions()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::vector<std::string> motionNames = scene->getMotionNames();
	_choiceTestMotions->clear();
	for (unsigned int i=0;i<motionNames.size();i++)
	{
		std::string moName = motionNames[i];		
		_choiceTestMotions->add(moName.c_str());
	}
}

void JointMapViewer::setTestMotion( std::string motionName )
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBMotion* motion = scene->getMotion(motionName);
	if (motion)
		testCommonSkMotion = motion;
}

void JointMapViewer::updateUI()
{
	updateTestMotions();
	//updateCharacterList();
	updateJointMapList();
	targetSkeletonViewer->updateSkeleton();
	
}

// void JointMapViewer::updateCharacterList()
// {
// 	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
// 	const std::vector<std::string>& characters = scene->getCharacterNames();
// 	_choiceCharacters->clear();
// 	for (size_t c = 0; c < characters.size(); c++)
// 	{
// 		_choiceCharacters->add(characters[c].c_str());
// 	}
// }

void JointMapViewer::updateJointMapList()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBJointMapManager* jointMapManager = scene->getJointMapManager();
	std::vector<std::string> jointMapNames = jointMapManager->getJointMapNames();	
	int oldValue = _choiceJointMaps->value();
	_choiceJointMaps->clear();
	_choiceJointMaps->add("----");
	for (size_t c = 0; c < jointMapNames.size(); c++)
	{
		_choiceJointMaps->add(jointMapNames[c].c_str());
	}

	if (oldValue < _choiceJointMaps->size() && oldValue >= 0)
	{
		_choiceJointMaps->value(oldValue);
	}
}


JointMapInputChoice::JointMapInputChoice( int x, int y, int w, int h, char* name ) : Fl_Input_Choice(x,y,w,h,name)
{
	skelViewer = NULL;
}

JointMapInputChoice::~JointMapInputChoice()
{

}

int JointMapInputChoice::handle( int event )
{
	int ret = -1;
	switch ( event )	
	{  
	case FL_PUSH:
		//LOG("widget %s is in focus",label());	
		ret = 1;
		break;
	}
	if (ret == 1 && skelViewer)
	{
		skelViewer->setFocusJointName(label());				
	}
	
	return Fl_Input_Choice::handle(event);
}

void JointMapInputChoice::setViewer( SkeletonViewer* viewer )
{
	skelViewer = viewer;
}


// ========================================================================
//    Name : time(time.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/01/13
// ========================================================================

#include "time.h"
static double g_Total = 0.0;
static double g_Delta = 0.0;

void Time_Update(double dt) { g_Delta = dt; g_Total += dt; }
double Time_GetTotal() { return g_Total; }
double Time_GetDelta() { return g_Delta; }
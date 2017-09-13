class TAxis;
class TCanvas;

void draw_axis_labels(TAxis* a);

//void graph_draw_stairs(TGraphAsymmErrors* g, int maxPoints = 99999);
void graph_draw_stairs(TGraphAsymmErrors* g, double ymin, double ymax);


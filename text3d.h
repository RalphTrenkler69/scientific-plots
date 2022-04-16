// text3d.h: Header file for class "text3d". Plot polygon text with ftgl and
// opengl.
#include <FTGL/ftgl.h>
#include <string>
#include <vector>

enum textpos_enum {left_lower, left_middle, left_upper, middle_lower,
	           middle_upper, right_lower, right_middle, right_upper};

class text3d {
private:
  FTPolygonFont& font;
  std::string text;
  textpos_enum textpos;
  std::vector<float> position, xvec, yvec, zvec;
 public:
  text3d(FTPolygonFont& font_arg, std::string text_arg, textpos_enum pos_arg,
	 std::vector<float> position_arg, std::vector<float> xvec_arg,
	 std::vector<float> yvec_arg, std::vector<float> zvec_arg)
    : font{font_arg},text{text_arg},textpos{pos_arg},position{position_arg},
      xvec{xvec_arg},yvec{yvec_arg},zvec{zvec_arg} {}
  text3d(FTPolygonFont& font_arg) : font{font_arg},text{""},textpos{left_lower},
      position{0.0,0.0,0.0},xvec{1.0,0.0,0.0},yvec{0.0,1.0,0.0},
      zvec{0.0,0.0,1.0} {}
  void txt(std::string t) {
    text = t;
  }
  void txtpos(textpos_enum pos) { textpos=pos; }
  void pos(std::vector<float> position_arg) {
    position = position_arg;
  }
  void xv(std::vector<float> vec) {
    xvec = vec;
  }
  void yv(std::vector<float> vec) {
    yvec = vec;
  }
  void zv(std::vector<float> vec) {
    zvec = vec;
  }
  void render();
private:
  void transform_textpos(FTBBox&);
  void rotscale_matrix(void);
};

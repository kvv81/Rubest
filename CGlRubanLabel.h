#ifndef __GL_RUBAN_LABEL__
#define __GL_RUBAN_LABEL__

/// Display list number. Used by glNewList
#define LABEL_DISPLAY_LIST 1

#define CORNER_RADIUS 0.25f
#define SECTORS_NUM_IN_CORNER 6

#define LABEL_HEIGHT 0.05f
#define LABEL_DEEPNESS 0.01f 

#define INNER_LABEL_SIZE /*0.7f*/  0.9f

/// RubanLabel class responsible for drawing one color label
/// on kub-rub by OpenGL
class CGlRubanLabel {
private:
	float _label_size; //< Label scaling size
	void GetArcCoord(int arc_num, int arc_step, float &x, float &y);
	void GetArcedQuadCoord(int arc_num, int arc_step, float &x, float &y);
public:
	CGlRubanLabel(float label_size);
	void Draw(float color_r, float color_g, float color_b);
};

#endif // __GL_RUBAN_LABEL__

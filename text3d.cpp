// text3d.cpp: Source file for text3d rendering function.
#include <iostream>
#include <GL/glut.h>
#include <FTGL/ftgl.h>
#include "text3d.h"

void text3d::transform_textpos(FTBBox& ftbox)
{
  //std::cout << "compute deltas for textpos..\n";
  float delta_x = ftbox.Upper().X()-ftbox.Lower().Y();
  float delta_y = ftbox.Upper().Y()-ftbox.Lower().Y();
  //std::cout << "do switch statement..\n";
  switch (textpos) {
  case left_lower:
    // do nothing.
    break;
  case left_middle:
    glTranslatef(0.0, -delta_y/2, 0.0);
    break;
  case left_upper:
    glTranslatef(0.0, -delta_y, 0.0);
    break;
  case middle_lower:
    glTranslatef(-delta_x/2, 0.0, 0.0);
    break;
  case middle_upper:
    glTranslatef(-delta_x/2, -delta_y, 0.0);
    break;
  case right_lower:
    glTranslatef(-delta_x, 0.0, 0.0);
    break;
  case right_middle:
    glTranslatef(-delta_x, -delta_y/2, 0.0);
    break;
  case right_upper:
    glTranslatef(-delta_x, -delta_y, 0.0);
    break;
  default:
    std::cout << "text3d error: unknown textpos.\n";
    break;
  }
}

void text3d::rotscale_matrix(void)
{
  float m[4][4];
  for (int i = 0; i < 3; i++)
    m[i][0] = xvec[i];
  for (int i = 0; i < 3; i++)
    m[i][1] = yvec[i];
  for (int i = 0; i < 3; i++)
    m[i][2] = zvec[i];
  for (int i = 0; i < 3; i++)
    m[i][3] = 0.0;
  for (int j = 0; j < 3; j++)
    m[3][j] = 0.0;
  m[3][3] = 1.0;
  glMultTransposeMatrixf(static_cast<float*>(&m[0][0]));
}

void text3d::render()
{
  FTBBox ftbox;
  //std::cout << "set face size..\n";
  font.FaceSize(1);
  //std::cout << "measure font box..\n";
  ftbox = font.BBox(text.c_str());
  glPushMatrix();
  glTranslatef(position[0],position[1],position[2]);
  //std::cout << "set rotation matrix..\n";
  rotscale_matrix();
  //std::cout << "set textpos matrix..\n";
  transform_textpos(ftbox);
  //std::cout << "render text..\n";
  font.Render(text.c_str());
  glPopMatrix();
}

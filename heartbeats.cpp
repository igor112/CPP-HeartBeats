#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string>

#define _USE_MATH_DEFINES

/* change these 20 parameters to affect the shape */
const double m_0[4] = {2.99, 2.99, 2.99, 2.99};
const double n1_0[4] = {0.40, 0.39, 0.38, 0.38};
const double n2_0[4] = {0.05, 0.06, 0.07, 0.1};
const double n3_0[4] = {3.0, 2.85, 2.7, 2.5};

const double m_ = 3.;
const double n1_ = 0.36;
const double n2_ = 0.15;
const double n3_ = 2.4;

/* change this one to affect quality */
const int Quality = 40;

/* change these 4 parameters to affect the period of heartbeats in each stage */
const double PERIOD_1 = 0.25;
const double PERIOD_2 = 0.35;
const double PERIOD_3 = 0.5;
const double PERIOD_4 = 1.0;

/* these 3 to change size */
const double Zoom_x = 1.5;
const double Zoom_y = 1.5;
const double Zoom_z = 1.5;

/* change these 4 parameters to affect the color of the heart in each stage */
const double COLOR_1 = 1.0;
const double COLOR_2 = 0.85;
const double COLOR_3 = 0.70;
const double COLOR_4 = 0.5;



/* system parameters */
double m = m_0[0];
double n1 = n1_0[0];
double n2 = n2_0[0];
double n3 = n3_0[0];



double color = COLOR_3;

double color_to_change = COLOR_3;
double time_for_coloring = 0;
double spent_time_for_coloring = 0;
double speed_of_coloring = 0;

int stage = 3;
double period = PERIOD_3;


bool is_opening = true;

GLfloat light_diffuse[] = {(float)COLOR_3, 0.0, 0.0, 0.9};
GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};


/* evaluate normal to the flat by its points */
void normal(double x1, double y1, double z1,
            double x2, double y2, double z2,  
            double x3, double y3, double z3,
            double* a, double* b, double* c) { 
    double a1 = x2 - x1;
    double b1 = y2 - y1;
    double c1 = z2 - z1;
    double a2 = x3 - x1;
    double b2 = y3 - y1;
    double c2 = z3 - z1;
    
    double _a = b1 * c2 - b2 * c1;
    double _b = a2 * c1 - a1 * c2;
    double _c = a1 * b2 - b1 * a2;

    double length = sqrt(_a*_a  + _b*_b + _c*_c);

    *a = _a / length;
    *b = _b / length;
    *c = _c / length;
} 


/* supershape formula */
double evaluate(double m, double n1, double n2, double n3, double phi) {
  double t1, t2;
  double a = 1, b = 1;

  t1 = cos(m * phi / 4) / a;
  t1 = fabs(t1);
  t1 = pow(t1, n2);

  t2 = sin(m * phi / 4) / b;
  t2 = fabs(t2);
  t2 = pow(t2, n3);

  if (t1 + t2 == 0.)
    return 0.;

  return pow(t1 + t2, -1 / n1);

}

/* generalization of the supershape formula */
void superformula(double m_1, double n_11, double n_12, double n_13,
                  double m_2, double n_21, double n_22, double n_23,
                  double theta, double phi, double *x, double *y, double *z) {

  double r_1 = evaluate(m_1, n_11, n_12, n_13, theta);
  double r_2 = evaluate(m_2, n_21, n_22, n_23, phi);

  *x = r_1 * cos(theta) * r_2 * cos(phi);
  *y = r_2 * sin(theta) * r_2 * cos(phi);
  *z = r_2 * sin(phi);
}


void drawHeart() {

  double x_1, y_1, z_1,
         x_2, y_2, z_2,
         x_3, y_3, z_3,
         a, b, c;

  glBegin(GL_TRIANGLES);
  for (int lat = -Quality; lat < Quality; ++lat) {
    double phi = lat * M_PI / 2 / Quality;
    double prev_phi = (lat - 1) * M_PI / 2 / Quality;

    for (int lon = -Quality; lon < Quality; ++lon) {
      double theta = lon * M_PI / Quality;
      double prev_theta = (lon - 1) * M_PI / Quality; 

      /*first triangle*/

      superformula(m, n1, n2, n3, m, n1, n2, n3,
                   theta, phi, &x_1, &y_1, &z_1);
      superformula(m, n1, n2, n3, m, n1, n2, n3,
                   prev_theta, phi, &x_2, &y_2, &z_2);
      superformula(m, n1, n2, n3, m, n1, n2, n3,
                   theta, prev_phi, &x_3, &y_3, &z_3);
      normal(x_1, y_1, z_1, x_2, y_2, z_2, x_3, y_3, z_3, &a, &b, &c);

      glBegin(GL_TRIANGLES);
      glNormal3f(a, b, c);
      glVertex3f(Zoom_x * x_1, Zoom_y * y_1, Zoom_z * z_1);
      glVertex3f(Zoom_x * x_2, Zoom_y * y_2, Zoom_z * z_2);
      glVertex3f(Zoom_x * x_3, Zoom_y * y_3, Zoom_z * z_3);
      glEnd();


      /*second triangle*/

      superformula(m, n1, n2, n3, m, n1, n2, n3,
                   prev_theta, prev_phi, &x_1, &y_1, &z_1);
      superformula(m, n1, n2, n3, m, n1, n2, n3,
                   theta, prev_phi, &x_2, &y_2, &z_2);
      superformula(m, n1, n2, n3, m, n1, n2, n3,
                   prev_theta, phi, &x_3, &y_3, &z_3);
      normal(x_1, y_1, z_1, x_2, y_2, z_2, x_3, y_3, z_3, &a, &b, &c);

      glBegin(GL_TRIANGLES);
      glNormal3f(a, b, c);
      glVertex3f(Zoom_x * x_1, Zoom_y * y_1, Zoom_z * z_1);
      glVertex3f(Zoom_x * x_2, Zoom_y * y_2, Zoom_z * z_2);
      glVertex3f(Zoom_x * x_3, Zoom_y * y_3, Zoom_z * z_3);
      glEnd();
    }
  }
  glEnd();

}

/* initialize openGL */
void init(void) {

  glEnable(GL_DEPTH_TEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

  glMatrixMode(GL_PROJECTION);
  gluPerspective(40.0, 1.0, 1.0, 10.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 5.0,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.);

  glTranslatef(0.0, 0.0, -1.0);
  glRotatef(60, 1.0, 0.0, 0.0);
  glRotatef(-20, 0.0, 0.0, 1.0);

  glLineWidth(2.0f);
}

void updateHeart(sf::Clock* clock, double* time, sf::Sound* sound) {
  sf::Time elapsed = clock->getElapsedTime();

  clock->restart();

  double delta_time = elapsed.asSeconds();

  if(is_opening == true) {
    if (*time < period) {
      *time += delta_time;
    } else {
      sound->play();
      *time = period;
      is_opening = false;
    }
  } else {
    if (*time > 0) {
      *time -= delta_time;
    } else {
      *time = 0;
      is_opening = true;
    }
  }

  if (color_to_change != color) {
    if (time_for_coloring == 0) {
      time_for_coloring = is_opening? period - *time + period : *time + period; 
      speed_of_coloring = (color_to_change - color) / time_for_coloring;
    }
    if (spent_time_for_coloring >= time_for_coloring) {
      time_for_coloring = 0;
      spent_time_for_coloring = 0;
      speed_of_coloring = 0;
      color = color_to_change; 
    } else {
      color += speed_of_coloring * delta_time;
      spent_time_for_coloring += delta_time;
    }
  }

  light_diffuse[0] = (float)color;
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);


  double t = *time / period;

  m = m_0[stage - 1] + (m_ - m_0[stage - 1]) * t;
  n1 = n1_0[stage - 1] + (n1_ - n1_0[stage - 1]) * t;
  n2 = n2_0[stage - 1] + (n2_ - n2_0[stage - 1]) * t;
  n3 = n3_0[stage - 1] + (n3_ - n3_0[stage - 1]) * t;

}

int main() {

  sf::SoundBuffer buffer;
  buffer.loadFromFile("Heartbeat.wav");

  sf::Sound sound;
  sound.setBuffer(buffer);
    
  sf::Clock clock;
  double time = 0;


  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antialiasingLevel = 4;
  settings.majorVersion = 3;
  settings.minorVersion = 0;

  sf::RenderWindow window(sf::VideoMode(800, 800), "Heartbeats", sf::Style::Default, settings);
  sf::RenderWindow textbox(sf::VideoMode(300, 100), "Enter stage:", sf::Style::Resize, settings);

  textbox.setPosition(textbox.getPosition() + sf::Vector2i(-200, 250));

  window.setVerticalSyncEnabled(true);

  std::string TextInput = "CASUAL";

  sf::Font font;
  font.loadFromFile("Arial.ttf");

  sf::Text text(TextInput, font, 24);

  window.setActive(true);
  sound.play();

  init();

  bool running = true;
  while (running) {

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        running = false;
        textbox.close();
      }
    }

    while (textbox.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        running = false;
        window.close();
      } else if (event.type == sf::Event::KeyPressed) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
          TextInput += "A";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
          TextInput += "B";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
          TextInput += "C";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
          TextInput += "D";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
          TextInput += "E";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
          TextInput += "F";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
          TextInput += "G";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
          TextInput += "H";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
          TextInput += "I";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
          TextInput += "J";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
          TextInput += "K";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
          TextInput += "L";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
          TextInput += "M";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
          TextInput += "N";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
          TextInput += "O";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
          TextInput += "P";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
          TextInput += "Q";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
          TextInput += "R";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
          TextInput += "S";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::T)) {
          TextInput += "T";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::U)) {
          TextInput += "U";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
          TextInput += "V";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
          TextInput += "W";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
          TextInput += "X";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y)) {
          TextInput += "Y";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
          TextInput += "Z";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
          TextInput += " ";
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace)) {
          if (TextInput.size() > 0) {
            TextInput.pop_back();
          }
        }
        text.setString(TextInput);

        if (TextInput == "BREAKDOWN" && stage != 1) {
          stage = 1;
          time = time * PERIOD_1 / period;
          period = PERIOD_1;
          if (color_to_change != color) {
            color = color_to_change;
            light_diffuse[0] = color;
            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
          }
          color_to_change = COLOR_1;
        } else if (TextInput == "MISGIVING" && stage != 2) {
          stage = 2;
          time = time * PERIOD_2 / period;
          period = PERIOD_2;
          if (color_to_change != color) {
            color = color_to_change;
            light_diffuse[0] = color;
            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
          }
          color_to_change = COLOR_2;
        } else if (TextInput == "CASUAL" && stage != 3) {
          stage = 3;
          time = time * PERIOD_3 / period;
          period = PERIOD_3;
          if (color_to_change != color) {
            color = color_to_change;
            light_diffuse[0] = color;
            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
          }
          color_to_change = COLOR_3;
        } else if (TextInput == "RECOVERY" && stage != 4) {
          stage = 4;
          time = time * PERIOD_4 / period;
          period = PERIOD_4;
          if (color_to_change != color) {
            color = color_to_change;
            light_diffuse[0] = color;
            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
          }
          color_to_change = COLOR_4;
        }
      }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    updateHeart(&clock, &time, &sound);
    drawHeart();

    window.display();

    textbox.setActive(true);

    textbox.clear();

    textbox.draw(text);

    textbox.display();

    window.setActive(true);

  }

  return 0;
}

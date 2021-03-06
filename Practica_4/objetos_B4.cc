//**************************************************************************
// Práctica 1 usando objetos
//**************************************************************************

#include "objetos_B4.h"
#include "file_ply_stl.hpp"

//*************************************************************************
// _puntos3D
//*************************************************************************

_puntos3D::_puntos3D()
{
}

//*************************************************************************
// dibujar puntos
//*************************************************************************

void _puntos3D::draw_puntos(float r, float g, float b, int grosor)
{
    int i;
    glPointSize(grosor);
    glColor3f(r, g, b);
    glBegin(GL_POINTS);
    for (i = 0; i < vertices.size(); i++)
    {
        glVertex3fv((GLfloat *)&vertices[i]);
    }
    glEnd();
}

//*************************************************************************
// _triangulos3D
//*************************************************************************

_triangulos3D::_triangulos3D()
{
    b_normales_caras = false;
    b_normales_vertices = false;

    ambiente_difusa = _vertex4f(0.2, 0.4, 0.9, 1.0); //coeficientes ambiente y difuso
    especular = _vertex4f(0.5, 0.5, 0.5, 1.0);       //coeficiente especular
    brillo = 50; 
}

//*************************************************************************
// dibujar en modo arista
//*************************************************************************

void _triangulos3D::draw_aristas(float r, float g, float b, int grosor)
{
    int i;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(grosor);
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
    for (i = 0; i < caras.size(); i++)
    {
        glVertex3fv((GLfloat *)&vertices[caras[i]._0]);
        glVertex3fv((GLfloat *)&vertices[caras[i]._1]);
        glVertex3fv((GLfloat *)&vertices[caras[i]._2]);
    }
    glEnd();
}

//*************************************************************************
// dibujar en modo sólido
//*************************************************************************

void _triangulos3D::draw_solido(float r, float g, float b)
{
    int i;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
    for (i = 0; i < caras.size(); i++)
    {
        glVertex3fv((GLfloat *)&vertices[caras[i]._0]);
        glVertex3fv((GLfloat *)&vertices[caras[i]._1]);
        glVertex3fv((GLfloat *)&vertices[caras[i]._2]);
    }
    glEnd();
}

//*************************************************************************
// dibujar en modo sólido con apariencia de ajedrez
//*************************************************************************

void _triangulos3D::draw_solido_ajedrez(float r1, float g1, float b1, float r2, float g2, float b2)
{
    int i;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);
    for (i = 0; i < caras.size(); i++)
    {
        if (i % 2 == 0)
            glColor3f(r1, g1, b1);
        else
            glColor3f(r2, g2, b2);

        glVertex3fv((GLfloat *)&vertices[caras[i]._0]);
        glVertex3fv((GLfloat *)&vertices[caras[i]._1]);
        glVertex3fv((GLfloat *)&vertices[caras[i]._2]);
    }
    glEnd();
}

//*************************************************************************
// dibujar iluminacion suave
//*************************************************************************

void _triangulos3D::draw_iluminacion_suave(){
    int i;
    
    if(!b_normales_vertices)
        calcular_normales_vertices(); //Calculamos las normales a los vertices si no se han calcuado ya

    if(!b_normales_caras)
        calcular_normales_caras(); //Calculamos las normales a las caras si no se han calcuado ya

    glEnable (GL_LIGHTING);
    glShadeModel(GL_SMOOTH);  //GL_SMOOTH
    glEnable(GL_NORMALIZE); //En caso de scale, si no, lo podemos quitar

    //Fijar color
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,(GLfloat *) &ambiente_difusa);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,(GLfloat *) &especular);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,brillo);

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_TRIANGLES);
    for (i=0;i<caras.size();i++){
        glNormal3fv((GLfloat *) &normales_vertices[caras[i]._0]);
        glVertex3fv((GLfloat *) &vertices[caras[i]._0]);
        glNormal3fv((GLfloat *) &normales_vertices[caras[i]._1]);
        glVertex3fv((GLfloat *) &vertices[caras[i]._1]);
        glNormal3fv((GLfloat *) &normales_vertices[caras[i]._2]);
        glVertex3fv((GLfloat *) &vertices[caras[i]._2]);
        }
    glEnd();

    glDisable(GL_LIGHTING);
}

//*************************************************************************
// dibujar iluminacion plana
//*************************************************************************

void _triangulos3D::draw_iluminacion_plana(){
  int i;
  if (b_normales_caras==false) calcular_normales_caras(); //Calculamos las normales a las caras si no se han calculado
  
  glEnable (GL_LIGHTING);
  glShadeModel(GL_FLAT);  //GL_SMOOTH
  glEnable(GL_NORMALIZE); //En caso de scale, si no, lo podemos quitar

  //Fijar color
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,(GLfloat *) &ambiente_difusa);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,(GLfloat *) &especular);
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,brillo);

  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glBegin(GL_TRIANGLES);
  for (i=0;i<caras.size();i++){
    glNormal3fv((GLfloat *) &normales_caras[i]);
    glVertex3fv((GLfloat *) &vertices[caras[i]._0]);     
    glVertex3fv((GLfloat *) &vertices[caras[i]._1]);
    glVertex3fv((GLfloat *) &vertices[caras[i]._2]);
    }
  glEnd();

  glDisable(GL_LIGHTING);
}

//*************************************************************************
// calcular normales a caras
//*************************************************************************

void _triangulos3D::calcular_normales_caras(){
    
  _vertex3f a1, a2, n;
  normales_caras.resize(caras.size());

  for(int i=0; i<caras.size(); i++){
	// obtener dos vectores en el triángulo y calcular el producto vectorial
	a1=vertices[caras[i]._1]-vertices[caras[i]._0];
       	a2=vertices[caras[i]._2]-vertices[caras[i]._0];
        n=a1.cross_product(a2);
	// modulo
	float m=sqrt(n.x*n.x+n.y*n.y+n.z*n.z);
	// normalización
    	normales_caras[i]= _vertex3f(n.x/m, n.y/m, n.z/m);
	}
    
  b_normales_caras=true;
}

//*************************************************************************
// calcular normales a vertices
//*************************************************************************

void _triangulos3D::calcular_normales_vertices(){

    normales_vertices.resize(vertices.size());

    for(int i=0; i<vertices.size(); i++){
        normales_vertices[i].x=0;
        normales_vertices[i].y=0;
        normales_vertices[i].z=0;
    }

    for (int i=0; i<caras.size(); i++){
        normales_vertices[caras[i]._0]+=normales_caras[i];
        normales_vertices[caras[i]._1]+=normales_caras[i];
        normales_vertices[caras[i]._2]+=normales_caras[i];
    }

    b_normales_vertices=true; 
}


//*************************************************************************
// dibujar con distintos modos
//*************************************************************************

void _triangulos3D::draw(_modo modo, float r1, float g1, float b1, float r2, float g2, float b2, float grosor)
{
    switch (modo)
    {
        case POINTS:
            draw_puntos(r1, g1, b1, grosor);
            break;
        case EDGES:
            draw_aristas(r1, g1, b1, grosor);
            break;
        case SOLID_CHESS:
            draw_solido_ajedrez(r1, g1, b1, r2, g2, b2);
            break;
        case SOLID:
            draw_solido(r1, g1, b1);
            break;
        case SOLID_ILLUMINATED_FLAT:
            draw_iluminacion_plana();
            break;
        case SOLID_ILLUMINATED_GOURAUD:
            draw_iluminacion_suave();
            break;
    }
}

//*************************************************************************
// clase cubo
//*************************************************************************

_cubo::_cubo(float tam)
{
    //vertices
    vertices.resize(8);
    vertices[0].x = -tam/2;
    vertices[0].y = -tam/2;
    vertices[0].z = -tam/2;
    vertices[1].x = -tam/2;
    vertices[1].y = -tam/2;
    vertices[1].z = tam/2;
    vertices[2].x = tam/2;
    vertices[2].y = -tam/2;
    vertices[2].z = tam/2;
    vertices[3].x = tam/2;
    vertices[3].y = -tam/2;
    vertices[3].z = -tam/2;
    vertices[4].x = tam/2;
    vertices[4].y = tam/2;
    vertices[4].z = -tam/2;
    vertices[5].x = -tam/2;
    vertices[5].y = tam/2;
    vertices[5].z = -tam/2;
    vertices[6].x = -tam/2;
    vertices[6].y = tam/2;
    vertices[6].z = tam/2;
    vertices[7].x = tam/2;
    vertices[7].y = tam/2;
    vertices[7].z = tam/2;

    // triangulos
    caras.resize(12);
    caras[0]._0 = 1;
    caras[0]._1 = 0;
    caras[0]._2 = 3;
    caras[1]._0 = 3;
    caras[1]._1 = 2;
    caras[1]._2 = 1;

    caras[2]._0 = 1;
    caras[2]._1 = 2;
    caras[2]._2 = 6;
    caras[3]._0 = 2;
    caras[3]._1 = 7;
    caras[3]._2 = 6;
    
    caras[4]._0 = 2;
    caras[4]._1 = 4;
    caras[4]._2 = 7;
    caras[5]._0 = 2;
    caras[5]._1 = 3;
    caras[5]._2 = 4;
    
    caras[6]._0 = 0;
    caras[6]._1 = 4;
    caras[6]._2 = 3;
    caras[7]._0 = 0;
    caras[7]._1 = 5;
    caras[7]._2 = 4;
    
    caras[8]._0 = 0;
    caras[8]._1 = 6;
    caras[8]._2 = 5;
    caras[9]._0 = 1;
    caras[9]._1 = 6;
    caras[9]._2 = 0;
    
    caras[10]._0 = 6;
    caras[10]._1 = 7;
    caras[10]._2 = 4;
    caras[11]._0 = 4;
    caras[11]._1 = 5;
    caras[11]._2 = 6;
}

//*************************************************************************
// clase piramide
//*************************************************************************

_piramide::_piramide(float tam, float al)
{

    //vertices
    vertices.resize(5);
    vertices[0].x = -tam;
    vertices[0].y = 0;
    vertices[0].z = tam;
    vertices[1].x = tam;
    vertices[1].y = 0;
    vertices[1].z = tam;
    vertices[2].x = tam;
    vertices[2].y = 0;
    vertices[2].z = -tam;
    vertices[3].x = -tam;
    vertices[3].y = 0;
    vertices[3].z = -tam;
    vertices[4].x = 0;
    vertices[4].y = al;
    vertices[4].z = 0;

    caras.resize(6);
    caras[0]._0 = 0;
    caras[0]._1 = 1;
    caras[0]._2 = 4;
    caras[1]._0 = 1;
    caras[1]._1 = 2;
    caras[1]._2 = 4;
    caras[2]._0 = 2;
    caras[2]._1 = 3;
    caras[2]._2 = 4;
    caras[3]._0 = 3;
    caras[3]._1 = 0;
    caras[3]._2 = 4;
    caras[4]._0 = 3;
    caras[4]._1 = 1;
    caras[4]._2 = 0;
    caras[5]._0 = 3;
    caras[5]._1 = 2;
    caras[5]._2 = 1;
}

//*************************************************************************
// clase cilindro
//*************************************************************************

_cilindro::_cilindro(float radio, int altura, int m)
{
    vector<_vertex3f> perfil1;
    _vertex3f aux;

        aux.x = radio;
        aux.y = - altura / 2.0;
        aux.z = 0;
        perfil1.push_back(aux);

        aux.y = altura / 2.0;
        perfil1.push_back(aux);

    parametros(perfil1, m, 0);
}

//*************************************************************************
// clase cono
//*************************************************************************

_cono::_cono(float radio, int altura, int m)
{
    vector<_vertex3f> perfil1;
    _vertex3f aux;

    aux.x = radio;
    aux.y = 0.0;
    aux.z = 0.0;
    perfil1.push_back(aux);

    aux.x = 0.0;
    aux.y = altura;
    aux.z = 0.0;
    perfil1.push_back(aux);

    parametros(perfil1, m, 1);
}

//*************************************************************************
// clase esfera
//*************************************************************************

_esfera::_esfera(float radio, int n, int m)
{
    vector<_vertex3f> perfil1;
    _vertex3f aux;

    for(int i = 1; i < n; i++){
        aux.x = radio * cos(M_PI * i / n - M_PI / 2.0);
        aux.y = radio * sin(M_PI * i / n - M_PI / 2.0);
        aux.z = 0.0;
        perfil1.push_back(aux);
    }

    parametros(perfil1, m, 2);

    normales_vertices.resize(vertices.size());
    for(int i=0; i<vertices.size(); i++){
        normales_vertices[i].x=vertices[i].x;
        normales_vertices[i].y=vertices[i].y;
        normales_vertices[i].z=vertices[i].z;
    }

    b_normales_vertices=true;
}

//*************************************************************************
// clase prisma trapezoidal
//*************************************************************************

_trapecio::_trapecio(float tam)
{

    ambiente_difusa = _vertex4f(1, 1, 1, 1.0); //coeficientes ambiente y difuso
    especular = _vertex4f(0.6, 0.6, 0.5, 1.0);       //coeficiente especular
    brillo = 32;

    //vertices
    vertices.resize(8);
    vertices[0].x = -tam/2*0.7; vertices[0].y = tam/2; vertices[0].z = tam/2;
    vertices[1].x = -tam/2; vertices[1].y = -tam/2; vertices[1].z = tam/2;
    vertices[2].x = tam/2; vertices[2].y = -tam/2; vertices[2].z = tam/2;
    vertices[3].x = tam/2 * 0.7; vertices[3].y = tam/2; vertices[3].z = tam/2;

    vertices[4].x = -tam/2 * 0.7; vertices[4].y = tam/2; vertices[4].z = -tam/2;
    vertices[5].x = -tam/2; vertices[5].y = -tam/2; vertices[5].z = -tam/2;
    vertices[6].x = tam/2; vertices[6].y = -tam/2; vertices[6].z = -tam/2;
    vertices[7].x = tam/2 * 0.7; vertices[7].y = tam/2; vertices[7].z = -tam/2;

    // caras
    caras.resize(12);
    caras[0]._0 = 0; caras[0]._1 = 1; caras[0]._2 = 3;
    caras[1]._0 = 1; caras[1]._1 = 2; caras[1]._2 = 3;
    
    caras[2]._0 = 3; caras[2]._1 = 2; caras[2]._2 = 6;
    caras[3]._0 = 3; caras[3]._1 = 6; caras[3]._2 = 7;

    caras[4]._0 = 0; caras[4]._1 = 3; caras[4]._2 = 7;
    caras[5]._0 = 0; caras[5]._1 = 7; caras[5]._2 = 4;

    caras[6]._0 = 1; caras[6]._1 = 0; caras[6]._2 = 4;
    caras[7]._0 = 1; caras[7]._1 = 4; caras[7]._2 = 5;

    caras[8]._0 = 5; caras[8]._1 = 4; caras[8]._2 = 7;
    caras[9]._0 = 5; caras[9]._1 = 7; caras[9]._2 = 6;

    caras[10]._0 = 1; caras[10]._1 = 2; caras[10]._2 = 6;
    caras[11]._0 = 1; caras[11]._1 = 6; caras[11]._2 = 5;
}

//*************************************************************************
// clase cabeza lego
//*************************************************************************

_cabeza_lego::_cabeza_lego()
{
    vector<_vertex3f> cabeza;
    _vertex3f aux;

    ambiente_difusa = _vertex4f(1, 1, 0.0, 1.0); //coeficientes ambiente y difuso
    especular = _vertex4f(0.6, 0.6, 0.5, 1.0);       //coeficiente especular
    brillo = 32; 

    //Mitad inferior
    aux.x = 0.2; aux.y = -0.5; aux.z = 0; cabeza.push_back(aux);
    aux.x = 0.2; aux.y = -0.45; cabeza.push_back(aux);
    aux.x = 0.225; aux.y = -0.43; cabeza.push_back(aux);
    aux.x = 0.25; aux.y = -0.4; cabeza.push_back(aux);
    aux.x = 0.3; aux.y = -0.35; cabeza.push_back(aux);
    aux.x = 0.35; aux.y = -0.3; cabeza.push_back(aux);
    aux.x = 0.4; aux.y = -0.25; cabeza.push_back(aux);
    aux.x = 0.45; aux.y = -0.15; cabeza.push_back(aux);

    //Mitad superior
    aux.x = 0.45; aux.y = 0.15; cabeza.push_back(aux);
    aux.x = 0.4; aux.y = 0.25; cabeza.push_back(aux);
    aux.x = 0.35; aux.y = 0.3; cabeza.push_back(aux);
    aux.x = 0.3; aux.y = 0.35; cabeza.push_back(aux);
    aux.x = 0.25; aux.y = 0.4; cabeza.push_back(aux);
    aux.x = 0.225; aux.y = 0.43; cabeza.push_back(aux);
    aux.x = 0.2; aux.y = 0.45; cabeza.push_back(aux);
    aux.x = 0.2; aux.y = 0.6; cabeza.push_back(aux);

    //Tubo superior
    aux.x = 0.15; aux.y = 0.6; cabeza.push_back(aux);
    aux.x = 0.15; aux.y = 0.4; cabeza.push_back(aux);

    // Puntos para crear el perfil de la cabeza
    parametros(cabeza, 30, 0);
}

// *************************************************************************
// clase ojo lego
// *************************************************************************

_ojo_lego::_ojo_lego(){
    pupila = new _esfera(1, 24, 24);
    pupila->ambiente_difusa = _vertex4f(0, 0, 0, 1.0); //coeficientes ambiente y difuso
    pupila->especular = _vertex4f(0, 0, 0, 1.0);       //coeficiente especular
    pupila->brillo = 32;
}

void _ojo_lego::draw(_modo modo, float r1, float g1, float b1, float r2, float g2, float b2, float grosor){
    //pupila
    glPushMatrix();
    glScalef(0.03, 0.03, 0.03);
    pupila->draw(modo, 0, 0, 0, r2, g2, b2, grosor);
    glPopMatrix();
}

// *************************************************************************
// clase tronco lego
// *************************************************************************

_tronco_lego::_tronco_lego(){}

void _tronco_lego::draw(_modo modo, float r1, float g1, float b1, float r2, float g2, float b2, float grosor){
    glPushMatrix();
    glScalef(1.6, 1.5, 0.5);
    tronco.draw(modo, r1, g1, b1, r2, g2, b2, grosor);
    glPopMatrix();
}

// *************************************************************************
// clase brazo lego
// *************************************************************************

_brazo_lego::_brazo_lego(){
    articulacion = new _cilindro(0.5, 1, 24);
    hombro = new _esfera(1, 24, 24);
    antebrazo = new _cilindro(0.5, 1, 24);

    articulacion->ambiente_difusa = _vertex4f(1, 1, 1, 1.0); //coeficientes ambiente y difuso
    articulacion->especular = _vertex4f(0.6, 0.6, 0.5, 1.0);       //coeficiente especular
    articulacion->brillo = 32;
    
    hombro->ambiente_difusa = _vertex4f(1, 1, 1, 1.0); //coeficientes ambiente y difuso
    hombro->especular = _vertex4f(0.6, 0.6, 0.5, 1.0);       //coeficiente especular
    hombro->brillo = 32;

    antebrazo->ambiente_difusa = _vertex4f(1, 1, 1, 1.0); //coeficientes ambiente y difuso
    antebrazo->especular = _vertex4f(0.6, 0.6, 0.5, 1.0);       //coeficiente especular
    antebrazo->brillo = 32;
}

void _brazo_lego::draw(_modo modo, float r1, float g1, float b1, float r2, float g2, float b2, float grosor){
    //Articulacion
    glPushMatrix();
    glTranslatef(0.1, 0.5, 0);
    glRotatef(90, 0, 0, 1);
    glScalef(0.3, 0.3, 0.3);
    articulacion->draw(modo, r1, g1, b1, r2, g2, b2, grosor);
    glPopMatrix();

    //Hombro
    glPushMatrix();
    glTranslatef(-0.05, 0.5, 0);
    glRotatef(-10, 0, 0, 1);
    glScalef(0.15, 0.15, 0.15);
    hombro->draw(modo, r1, g1, b1, r2, g2, b2, grosor);
    glPopMatrix();

    //Antebrazo
    glPushMatrix();
    glTranslatef(-0.15, -0.1, 0);
    glRotatef(-10, 0, 0, 1);
    glScalef(0.3, 1.2, 0.3);
    antebrazo->draw(modo, r1, g1, b1, r2, g2, b2, grosor);
    glPopMatrix();
}

// *************************************************************************
// mano lego
// *************************************************************************

_mano_lego::_mano_lego(){
    munheca = new _cilindro(0.5, 1, 24);
    palma = new _cubo(1);
    dedos_izq = new _cubo(1);
    dedos_der = new _cubo(1);

    munheca->ambiente_difusa = _vertex4f(1, 1, 0.0, 1.0); //coeficientes ambiente y difuso
    munheca->especular = _vertex4f(0.6, 0.6, 0.5, 1.0);       //coeficiente especular
    munheca->brillo = 32;

    palma->ambiente_difusa = _vertex4f(1, 1, 0.0, 1.0); //coeficientes ambiente y difuso
    palma->especular = _vertex4f(0.6, 0.6, 0.5, 1.0);       //coeficiente especular
    palma->brillo = 32;

    dedos_izq->ambiente_difusa = _vertex4f(1, 1, 0.0, 1.0); //coeficientes ambiente y difuso
    dedos_izq->especular = _vertex4f(0.6, 0.6, 0.5, 1.0);       //coeficiente especular
    dedos_izq->brillo = 32;

    dedos_der->ambiente_difusa = _vertex4f(1, 1, 0.0, 1.0); //coeficientes ambiente y difuso
    dedos_der->especular = _vertex4f(0.6, 0.6, 0.5, 1.0);       //coeficiente especular
    dedos_der->brillo = 32;
}

void _mano_lego::draw(_modo modo, float r1, float g1, float b1, float r2, float g2, float b2, float grosor){

    //Muñeca
    glPushMatrix();
    glScalef(0.2, 0.1, 0.2);
    munheca->draw(modo, r1, g1, b1, r2, g2, b2, grosor);
    glPopMatrix();

    //Palma
    glPushMatrix();
    glTranslatef(0, -0.1, 0);
    glScalef(0.3, 0.1, 0.2);
    palma->draw(modo, r1, g1, b1, r2, g2, b2, grosor);
    glPopMatrix();

    //Dedos izquierdos
    glPushMatrix();
    glTranslatef(-0.1, -0.25, 0);
    glScalef(0.1, 0.2, 0.2);
    dedos_izq->draw(modo, r1, g1, b1, r2, g2, b2, grosor);
    glPopMatrix();

    //Dedos derechos
    glPushMatrix();
    glTranslatef(0.1, -0.25, 0);
    glScalef(0.1, 0.2, 0.2);
    dedos_der->draw(modo, r1, g1, b1, r2, g2, b2, grosor);
    glPopMatrix();

}

// *************************************************************************
// clase pelvis lego
// *************************************************************************

_pelvis_lego::_pelvis_lego(){
    cintura = new _cubo(1);
    ingle = new _cilindro(0.5, 1, 24);
    articulacion_piernas = new _cilindro(0.5, 1, 24);

    cintura->ambiente_difusa = _vertex4f(0.775, 0.459, 0.2, 1.0); //coeficientes ambiente y difuso
    cintura->especular = _vertex4f(0.775, 0.459, 0.2, 1.0);       //coeficiente especular
    cintura->brillo = 25.6;

    ingle->ambiente_difusa = _vertex4f(51/255, 102/255, 1, 1.0); //coeficientes ambiente y difuso
    ingle->especular = _vertex4f(0.6, 0.6, 0.5, 1.0);       //coeficiente especular
    ingle->brillo = 32;

    articulacion_piernas->ambiente_difusa = _vertex4f(51/255, 102/255, 1, 1.0); //coeficientes ambiente y difuso
    articulacion_piernas->especular = _vertex4f(0.6, 0.6, 0.5, 1.0);       //coeficiente especular
    articulacion_piernas->brillo = 32;
}

void _pelvis_lego::draw(_modo modo, float r1, float g1, float b1, float r2, float g2, float b2, float grosor){

    //Ingle
    glPushMatrix();
    glTranslatef(0, -0.25, 0);
    glRotatef(90, 0, 0, 1);
    glScalef(0.4, 0.3, 0.4);
    ingle->draw(modo, r1, g1, b1, r2, g2, b2, grosor);
    glPopMatrix();

    //Articulacion piernas
    glPushMatrix();
    glTranslatef(0, -0.25, 0);
    glRotatef(90, 0, 0, 1);
    glScalef(0.2, 1.5, 0.2);
    articulacion_piernas->draw(modo, r1, g1, b1, r2, g2, b2, grosor);
    glPopMatrix();

    //Cintura
    glPushMatrix();
    glScalef(1.62, 0.25, 0.5);
    cintura->draw(modo, 153.0/255, 102.0/255, 51.0/255, 153.0/255 - 0.1, 102.0/255 - 0.1, 51.0/255 - 0.1, grosor);
    glPopMatrix();
}

// *************************************************************************
// clase pierna lego
// *************************************************************************

_pierna_lego::_pierna_lego(){
    cuadriceps = new _cilindro(0.5, 1, 24);
    gemelo = new _cubo(1);
    pie = new _cubo(1);

    cuadriceps->ambiente_difusa = _vertex4f(51/255, 102/255, 1, 1.0); //coeficientes ambiente y difuso
    cuadriceps->especular = _vertex4f(0.6, 0.6, 0.5, 1.0);       //coeficiente especular
    cuadriceps->brillo = 32;

    gemelo->ambiente_difusa = _vertex4f(51/255, 102/255, 1, 1.0); //coeficientes ambiente y difuso
    gemelo->especular = _vertex4f(0.6, 0.6, 0.5, 1.0);       //coeficiente especular
    gemelo->brillo = 32;

    pie->ambiente_difusa = _vertex4f(0.25, 0.23, 0.103, 1.0); //coeficientes ambiente y difuso
    pie->especular = _vertex4f(0.775, 0.459, 0.2, 1.0);       //coeficiente especular
    pie->brillo = 25.6;
}

void _pierna_lego::draw(_modo modo, float r1, float g1, float b1, float r2, float g2, float b2, float grosor){
    
    //cuadricep
    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    glScalef(0.5, 0.7, 0.5);
    cuadriceps->draw(modo, r1, g1, b1, r2, g2, b2, grosor);
    glPopMatrix();

    //gemelo
    glPushMatrix();
    glTranslatef(0, -0.5, 0);
    glScalef(0.7, 0.8, 0.5);
    gemelo->draw(modo, r1, g1, b1, r2, g2, b2, grosor);
    glPopMatrix();

    //pie
    glPushMatrix();
    glTranslatef(0, -0.9, 0.14);
    glScalef(0.71, 0.2, 0.8);
    gemelo->draw(modo, 153.0/255, 102.0/255, 51.0/255, 153.0/255 - 0.1, 102.0/255 - 0.1, 51.0/255 - 0.1, grosor);
    glPopMatrix();
}

// *************************************************************************
// clases lego
// *************************************************************************

_lego::_lego(){
    //Giro cabeza
    giro_cabeza = 0.0;
    giro_cabeza_min = -45.0;
    giro_cabeza_max = 45.0;

    //Giro brazos
    giro_brazo_izq = 0.0;
    giro_brazo_der = 0.0;
    giro_brazo_min = 45.0;
    giro_brazo_max = -170.0;

    //Giro piernas
    giro_pierna_izq = 0.0;
    giro_pierna_der = 0.0;
    giro_pierna_min = 45.0;
    giro_pierna_max = -90.0;
}

void _lego::draw(_modo modo, float r1, float g1, float b1, float r2, float g2, float b2, float grosor){

    glPushMatrix();
    glTranslatef(0, 2.13, 0);
        //Giro cabeza
        glPushMatrix();
        glRotatef(giro_cabeza, 0, 1, 0);
            //Cabeza
            glPushMatrix();
            glTranslatef(0, 1.3, 0);
            glScalef(1.1, 1.1, 1.1);
            cabeza.draw(modo, 1, 1, 0, 0.9, 0.9, 0, grosor);
            glPopMatrix();

            //Ojo izquierdo
            glPushMatrix();
            glTranslatef(-0.15, 1.33, 0.5);
            ojo_izquierdo.draw(modo, 0, 0, 0, 0, 0, 0, grosor);
            glPopMatrix();

            //Ojo derecho
            glPushMatrix();
            glTranslatef(0.15, 1.33, 0.5);
            ojo_derecho.draw(modo, 0, 0, 0, 0, 0, 0, grosor);
            glPopMatrix();
        glPopMatrix();

        //Tronco
        glPushMatrix();
        tronco.draw(modo, r1, g1, b1, r2, g2, b2, grosor);
        glPopMatrix();

        //Giro brazo izquierdo
        glPushMatrix();
        glTranslatef(-0.6, 0.5, 0);
        glRotatef(giro_brazo_izq, 1, 0, 0);
        glTranslatef(0.6, -0.5, 0);
            //Brazo izquierdo
            glPushMatrix();
            glTranslatef(-0.75, 0, 0);
            brazo_izquierdo.draw(modo, r1, g1, b1, r2, g2, b2, grosor);
            glPopMatrix();

            //Mano izquierda
            glPushMatrix();
            glTranslatef(-1.02, -0.73, 0);
            glRotatef(-10, 0, 0, 1);
            mano_izquierda.draw(modo, 1, 1, 0, 0.9, 0.9, 0, grosor);
            glPopMatrix();
        glPopMatrix();
        
        //Giro brazo izquierdo
        glPushMatrix();
        glTranslatef(0.6, 0.5, 0);
        glRotatef(giro_brazo_der, 1, 0, 0);
        glTranslatef(-0.6, -0.5, 0);
            //Brazo derecho
            glPushMatrix();
            glTranslatef(0.75, 0, 0);
            glRotatef(180, 0, 1, 0);
            brazo_derecho.draw(modo, r1, g1, b1, r1, g2, b2, grosor);
            glPopMatrix();

            //Mano derecha
            glPushMatrix();
            glTranslatef(1.02, -0.73, 0);
            glRotatef(180, 0, 1, 0);
            glRotatef(-10, 0, 0, 1);
            mano_derecha.draw(modo, 1, 1, 0, 0.9, 0.9, 0, grosor);
            glPopMatrix();
        glPopMatrix();

        //Pelvis
        glPushMatrix();
        glTranslatef(0, -0.875, 0);
        pelvis.draw(modo, 51/255, 102/255, 1, 51/255 - 0.1, 102/255 - 0.1, 0.9, grosor);
        glPopMatrix();

        //Pierna izquierda
        glPushMatrix();
        glTranslatef(-0.45, -1.125, 0);
        glRotatef(giro_pierna_izq, 1, 0, 0);
        pierna_izquierda.draw(modo, 51/255, 102/255, 1, 51/255 - 0.1, 102/255 - 0.1, 0.9, grosor);
        glPopMatrix();

        //Pierna derecha
        glPushMatrix();
        glTranslatef(0.45, -1.125, 0);
        glRotatef(giro_pierna_der, 1, 0, 0);
        pierna_derecha.draw(modo, 51/255, 102/255, 1, 51/255 - 0.1, 102/255 - 0.1, 0.9, grosor);
        glPopMatrix();
    glPopMatrix();
}

//*************************************************************************
// clase objeto ply
//*************************************************************************

_objeto_ply::_objeto_ply()
{
    // leer lista de coordenadas de vértices y lista de indices de vértices
}

int _objeto_ply::parametros(char *archivo)
{
    int n_ver, n_car;

    vector<float> ver_ply;
    vector<int> car_ply;

    _file_ply::read(archivo, ver_ply, car_ply);

    n_ver = ver_ply.size() / 3;
    n_car = car_ply.size() / 3;

    printf("Number of vertices=%d\nNumber of faces=%d\n", n_ver, n_car);

    vertices.resize(n_ver);

    for (int i = 0; i < n_ver; i++)
    {
        vertices[i].x = ver_ply[3 * i];
        vertices[i].y = ver_ply[3 * i + 1];
        vertices[i].z = ver_ply[3 * i + 2];
    }

    caras.resize(n_car);

    for (int i = 0; i < n_car; i++)
    {
        caras[i]._0 = car_ply[3 * i];
        caras[i]._1 = car_ply[3 * i + 1];
        caras[i]._2 = car_ply[3 * i + 2];
    }

    return (0);
}

//************************************************************************
// objeto por revolucion
//************************************************************************

_rotacion::_rotacion()
{
}

void _rotacion::parametros(vector<_vertex3f> perfil, int num, int tipo)
{
    int i, j;
    _vertex3f vertice_aux;
    _vertex3i cara_aux;
    int num_aux;
    float radio, altura;

    // Para cilindro y esfera:
    if(tipo == 0 || tipo == 2){
        num_aux = perfil.size();
    }

    // Para cono:
    if(tipo == 1){
        num_aux = 1;
        altura = perfil[1].y;
    }

    // Para esfera:
    if(tipo == 2)
        radio = sqrt(perfil[0].x*perfil[0].x+perfil[0].y*perfil[0].y);

    // tratamiento de los vértice
    vertices.resize(num_aux * num);

    for (j = 0; j < num; j++){
        for (i = 0; i < num_aux; i++){
            vertice_aux.x = perfil[i].x * cos(2.0 * M_PI * j / (1.0 * num)) +
                            perfil[i].z * sin(2.0 * M_PI * j / (1.0 * num));
            vertice_aux.z = -perfil[i].x * sin(2.0 * M_PI * j / (1.0 * num)) +
                            perfil[i].z * cos(2.0 * M_PI * j / (1.0 * num));
            vertice_aux.y = perfil[i].y;
            vertices[i + j * num_aux] = vertice_aux;
        }
    }

    int c = 0;

    caras.resize(2 * (num_aux - 1) * num);

    // tratamiento de las caras
    if(tipo != 1){
        for(int j = 0; j < num - 1; j++){
            for(int i = 0; i < num_aux - 1; i++){
                caras[c]._0 = j * num_aux + i;
                caras[c]._2 = ( j + 1 ) * num_aux + i + 1;
                caras[c]._1 = ( j + 1 ) * num_aux + i;

                c++;

                caras[c]._0 = j * num_aux + i;
                caras[c]._2 = j * num_aux + i + 1;
                caras[c]._1 = ( j + 1 ) * num_aux + i + 1;

                c++;
            }
        }

        // Tratamiento para las ultimas caras
        for(int i = 0; i < num_aux - 1; i++){
            caras[c]._0 = num * num_aux - num_aux + i;
            caras[c]._2 = i + 1;
            caras[c]._1 = i;

            c++;

            caras[c]._0 = num * num_aux - num_aux + i;
            caras[c]._2 = num * num_aux - (num_aux - i - 1);
            caras[c]._1 = i + 1;

            c++;
        }
    }

    // tapa superior
    //Redimiensiono para añadir las nuevas caras y el vértice
    vertices.resize(vertices.size() + 1);
    caras.resize(caras.size() + num);

    // Vertice central de tapa superior
    vertices[vertices.size() - 1].x = 0;

    if(tipo == 0)
        vertices[vertices.size() - 1].y = perfil[num_aux - 1].y;
    if(tipo == 1)
        vertices[vertices.size() - 1].y = altura;
    if(tipo == 2)
        vertices[vertices.size() - 1].y = radio;

    vertices[vertices.size() - 1].z = 0;

    if (fabs(perfil[num_aux - 1].x) > 0.0)
    {
        for(int i = 0; i < num - 1; i++){
            caras[c]._0 = (i + 1) * num_aux - 1;
            caras[c]._2 = vertices.size() - 1;
            caras[c]._1 = (i + 2) * num_aux - 1;

            c++;
        }

        caras[c]._0 = num_aux * num - 1;
        caras[c]._2 = vertices.size() - 1;
        caras[c]._1 = num_aux - 1;

        c++;
    }

    // tapa inferior
    //Redimiensiono para añadir las nuevas caras y el vértice
    vertices.resize(vertices.size() + 1);
    caras.resize(caras.size() + num);

    // Vertice central de tapa inferior
    vertices[vertices.size() - 1].x = 0;
    if(tipo == 0 || tipo == 1)
        vertices[vertices.size() - 1].y = vertices[0].y;
    if(tipo == 2)
        vertices[vertices.size() - 1].y = -radio;
    vertices[vertices.size() - 1].z = 0;

    if (fabs(perfil[0].x) > 0.0)
    {
        for(int i = 0; i < num - 1; i++){
            caras[c]._0 = i * num_aux;
            caras[c]._2 = vertices.size() - 1;
            caras[c]._1 = num_aux * (i + 1);

            c++;
        }

        caras[c]._0 = 0;
        caras[c]._2 = vertices.size() - 1;
        caras[c]._1 = num_aux * (num - 1);

        c++;

    }
}

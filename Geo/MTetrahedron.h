// Gmsh - Copyright (C) 1997-2009 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to <gmsh@geuz.org>.

#ifndef _MTETRAHEDRON_H_
#define _MTETRAHEDRON_H_

#include "MElement.h"

/*
 * MTetrahedron
 *
 *                      v
 *                    .
 *                  ,/
 *                 /
 *              2
 *            ,/|`\
 *          ,/  |  `\
 *        ,/    '.   `\
 *      ,/       |     `\
 *    ,/         |       `\
 *   0-----------'.--------1 --> u
 *    `\.         |      ,/
 *       `\.      |    ,/
 *          `\.   '. ,/
 *             `\. |/
 *                `3
 *                   `\.
 *                      ` w
 *
 */
class MTetrahedron : public MElement {
 protected:
  MVertex *_v[4];
  void _getEdgeVertices(const int num, std::vector<MVertex*> &v) const
  {
    v[0] = _v[edges_tetra(num, 0)];
    v[1] = _v[edges_tetra(num, 1)];
  }
  void _getFaceVertices(const int num, std::vector<MVertex*> &v) const
  {
    v[0] = _v[faces_tetra(num, 0)];
    v[1] = _v[faces_tetra(num, 1)];
    v[2] = _v[faces_tetra(num, 2)];
  }
 public :
  MTetrahedron(MVertex *v0, MVertex *v1, MVertex *v2, MVertex *v3, int num=0, int part=0) 
    : MElement(num, part)
  {
    _v[0] = v0; _v[1] = v1; _v[2] = v2; _v[3] = v3;
  }
  MTetrahedron(std::vector<MVertex*> &v, int num=0, int part=0) 
    : MElement(num, part)
  {
    for(int i = 0; i < 4; i++) _v[i] = v[i];
  }
  ~MTetrahedron(){}
  virtual int getDim(){ return 3; }
  virtual int getNumVertices() const { return 4; }
  virtual MVertex *getVertex(int num){ return _v[num]; }
  virtual MVertex *getVertexMED(int num)
  {
    static const int map[4] = {0, 2, 1, 3};
    return getVertex(map[num]);
  }
  virtual int getNumEdges(){ return 6; }
  virtual MEdge getEdge(int num)
  {
    return MEdge(_v[edges_tetra(num, 0)], _v[edges_tetra(num, 1)]);
  }
  virtual int getNumEdgesRep(){ return 6; }
  virtual void getEdgeRep(int num, double *x, double *y, double *z, SVector3 *n)
  {
    static const int f[6] = {0, 0, 0, 1, 2, 3};
    MEdge e(getEdge(num));
    _getEdgeRep(e.getVertex(0), e.getVertex(1), x, y, z, n, f[num]);
  }
  virtual void getEdgeVertices(const int num, std::vector<MVertex*> &v) const
  {
    v.resize(2);
    _getEdgeVertices(num, v);
  }
  virtual int getNumFaces(){ return 4; }
  virtual MFace getFace(int num)
  {
    return MFace(_v[faces_tetra(num, 0)],
                 _v[faces_tetra(num, 1)],
                 _v[faces_tetra(num, 2)]);
  }
  virtual void getFaceInfo (const MFace & face, int &ithFace, int &sign, int &rot)const; 
  virtual int getNumFacesRep(){ return 4; }
  virtual void getFaceRep(int num, double *x, double *y, double *z, SVector3 *n)
  { 
    MFace f(getFace(num));
    _getFaceRep(f.getVertex(0), f.getVertex(1), f.getVertex(2), x, y, z, n);
  }
  virtual void getFaceVertices(const int num, std::vector<MVertex*> &v) const
  {
    v.resize(3);
    _getFaceVertices(num, v);
  }
  virtual int getType() const { return TYPE_TET; }
  virtual int getTypeForMSH() const { return MSH_TET_4; }
  virtual int getTypeForUNV() const { return 111; } // solid linear tetrahedron
  virtual int getTypeForVTK() const { return 10; }
  virtual const char *getStringForPOS() const { return "SS"; }
  virtual const char *getStringForBDF() const { return "CTETRA"; }
  virtual const char *getStringForDIFF() const { return "ElmT4n3D"; }
  virtual void revert()
  {
    MVertex *tmp = _v[0]; _v[0] = _v[1]; _v[1] = tmp;
  }
  void getMat(double mat[3][3])
  {
    mat[0][0] = _v[1]->x() - _v[0]->x();
    mat[0][1] = _v[2]->x() - _v[0]->x();
    mat[0][2] = _v[3]->x() - _v[0]->x();
    mat[1][0] = _v[1]->y() - _v[0]->y();
    mat[1][1] = _v[2]->y() - _v[0]->y();
    mat[1][2] = _v[3]->y() - _v[0]->y();
    mat[2][0] = _v[1]->z() - _v[0]->z();
    mat[2][1] = _v[2]->z() - _v[0]->z();
    mat[2][2] = _v[3]->z() - _v[0]->z();
  }
  virtual double getVolume();
  virtual int getVolumeSign(){ return (getVolume() >= 0) ? 1 : -1; }
  virtual double gammaShapeMeasure();
  virtual double distoShapeMeasure();
  virtual double etaShapeMeasure();
  void xyz2uvw(double xyz[3], double uvw[3]);
  virtual const functionSpace* getFunctionSpace(int o=-1) const;
  virtual bool isInside(double u, double v, double w)
  {
    double tol = _isInsideTolerance;
    if(u < (-tol) || v < (-tol) || w < (-tol) || u > ((1. + tol) - v - w))
      return false;
    return true;
  }
  virtual void getIntegrationPoints(int pOrder, int *npts, IntPt **pts);
  virtual SPoint3 circumcenter();
 private:
  int edges_tetra(const int edge, const int vert) const
  {
    static const int e[6][2] = {
      {0, 1},
      {1, 2},
      {2, 0},
      {3, 0},
      {3, 2},
      {3, 1}
    };
    return e[edge][vert];
  }
  int faces_tetra(const int face, const int vert) const
  {
    static const int f[4][3] = {
      {0, 2, 1},
      {0, 1, 3},
      {0, 3, 2},
      {3, 1, 2}
    };
    return f[face][vert];
  }
};

/*
 * MTetrahedron10
 *
 *              2 
 *            ,/|`\
 *          ,/  |  `\
 *        ,6    '.   `5
 *      ,/       8     `\
 *    ,/         |       `\
 *   0--------4--'.--------1
 *    `\.         |      ,/
 *       `\.      |    ,9
 *          `7.   '. ,/
 *             `\. |/
 *                `3
 *
 */
class MTetrahedron10 : public MTetrahedron {
 protected:
  MVertex *_vs[6];
 public :
  MTetrahedron10(MVertex *v0, MVertex *v1, MVertex *v2, MVertex *v3, MVertex *v4, 
                 MVertex *v5, MVertex *v6, MVertex *v7, MVertex *v8, MVertex *v9,
                 int num=0, int part=0) 
    : MTetrahedron(v0, v1, v2, v3, num, part)
  {
    _vs[0] = v4; _vs[1] = v5; _vs[2] = v6; _vs[3] = v7; _vs[4] = v8; _vs[5] = v9;
    for(int i = 0; i < 6; i++) _vs[i]->setPolynomialOrder(2);
  }
  MTetrahedron10(std::vector<MVertex*> &v, int num=0, int part=0) 
    : MTetrahedron(v, num, part)
  {
    for(int i = 0; i < 6; i++) _vs[i] = v[4 + i];
    for(int i = 0; i < 6; i++) _vs[i]->setPolynomialOrder(2);
  }
  ~MTetrahedron10(){}
  virtual int getPolynomialOrder() const { return 2; }
  virtual int getNumVertices() const { return 10; }
  virtual MVertex *getVertex(int num){ return num < 4 ? _v[num] : _vs[num - 4]; }
  virtual MVertex *getVertexUNV(int num)
  {
    static const int map[10] = {0, 4, 1, 5, 2, 6, 7, 9, 8, 3};
    return getVertex(map[num]); 
  }
  virtual MVertex *getVertexBDF(int num)
  {
    static const int map[10] = {0, 1, 2, 3, 4, 5, 6, 7, 9, 8};
    return getVertex(map[num]); 
  }
  virtual MVertex *getVertexMED(int num)
  {
    static const int map[10] = {0, 2, 1, 3, 6, 5, 4, 7, 8, 9};
    return getVertex(map[num]); 
  }
  virtual MVertex *getVertexDIFF(int num)
  {
    static const int map[10] = {0, 1, 2, 3, 4, 5, 6, 7, 9, 8};
    return getVertex(map[num]); 
  }
  virtual int getNumEdgeVertices() const { return 6; }

  virtual void getEdgeRep(int num, double *x, double *y, double *z, SVector3 *n);
  virtual int getNumEdgesRep();
  virtual void getFaceRep(int num, double *x, double *y, double *z, SVector3 *n);
  virtual int getNumFacesRep();

  virtual void getEdgeVertices(const int num, std::vector<MVertex*> &v) const
  {
    v.resize(3);
    MTetrahedron::_getEdgeVertices(num, v);
    v[2] = _vs[num];
  }

  virtual void getFaceVertices(const int num, std::vector<MVertex*> &v) const
  {
    v.resize(6);
    MTetrahedron::_getFaceVertices(num, v);
    static const int f[4][3] = {
      {4, 5, 6},
      {4, 7, 9},
      {6, 7, 8},
      {5, 8, 9}
    };
    v[3] = _vs[f[num][0]];
    v[4] = _vs[f[num][1]];
    v[5] = _vs[f[num][2]];
  }
  virtual int getTypeForMSH() const { return MSH_TET_10; }
  virtual int getTypeForUNV() const { return 118; } // solid parabolic tetrahedron
  //virtual int getTypeForVTK() const { return 24; }
  virtual const char *getStringForPOS() const { return "SS2"; }
  virtual const char *getStringForBDF() const { return "CTETRA"; }
  virtual const char *getStringForDIFF() const { return "ElmT10n3D"; }
  virtual void revert()
  {
    MVertex *tmp;
    tmp = _v[0] ; _v[0]  = _v[1]; _v[1] = tmp;
    tmp = _vs[1]; _vs[1] = _vs[2]; _vs[2] = tmp;
    tmp = _vs[5]; _vs[5] = _vs[3]; _vs[3] = tmp;
  }
};

/*
 * MTetrahedronN  FIXME: check the plot
 *
 *              2
 *            ,/|`\ 
 *          ,/  |  `\              E = order - 1
 *        ,/    '.   `\            C = 4 + 6*E 
 *      ,/       |     `\          F = ((order - 1)*(order - 2))/2
 *    ,/         |       `\        N = total number of vertices
 *   0-----------'.--------1
 *    `\.         |      ,/        Interior vertex numbers  
 *       `\.      |    ,/            for edge 0 <= i <= 5: 4+i*E to 3+(i+1)*E
 *          `\.   '. ,/              for face 0 <= j <= 3: C+j*F to C-1+(j+1)*F
 *             `\. |/                in volume           : C+4*F to N-1
 *                `3
 *
 */



/* tet order 3
   
 *              2
 *            ,/|`\ 
 *          ,5  |  `6              E = order - 1
 *        ,/    12   `\            C = 4 + 6*E 
 *      ,4       |     `7          F = ((order - 1)*(order - 2))/2
 *    ,/         |       `\        N = total number of vertices
 *   0-----9-----'.--8-----1
 *    `\.         |      ,/        Interior vertex numbers
 *       10.     13    ,14           for edge 0 <= i <= 5: 4+i*E to 3+(i+1)*E
 *          `\.   '. 15              for face 0 <= j <= 3: C+j*F to C-1+(j+1)*F
 *             11\.|/        in volume           : C+4*F to N-1
 *                `3
 *
 */

static int reverseTet20[20] = {0,2,1,3,  // principal vertices
                               9,8,      // E0 switches with E2
                               7,6,      // E1 inverts direction
                               5,4,      // E2 switches with E0
                               10,11,    // E3 pure w edge > remains the same
                               14,15,    // E4 uw edge swithes with v/w edge E5
                               12,13,    // E5 switches with E4
                               16,       // F0 is uv plane, reverts normal
                               18,       // F1 is uw plane, switches with F2
                               17,       // F2 is vw plane, switches with F1
                               19};      // F3 is uvw plane, reverts normal

static int reverseTet35[35] = {0,2,1,3,  // principal vertices
                               
                               12,11,10, // E0 switches with E2
                               9,8,7,    // E1 inverts direction
                               6,5,4,    // E2 switches with E0
                               13,14,15, // E3 pure w edge > remains the same
                               19,20,21, // E4 uw edge swithes with v/w edge E5
                               16,17,18, // E5 switches with E4
                               22,24,23, // F0 is uv plane, reverts normal
                               28,30,29, // F1 is uw plane, switches with F2, orientation is different
                               25,27,26, // F2 is vw plane, switches with F1
                               31,33,32, // F3 is uvw plane, reverts normal
                               34};      // central node remains 
  
static int reverseTet34[34] = {0,2,1,3,  // principal vertices
                               12,11,10, // E0 switches with E2
                               9,8,7,    // E1 inverts direction
                               6,5,4,    // E2 switches with E0
                               13,14,15, // E3 pure w edge > remains the same
                               19,20,21, // E4 uw edge swithes with v/w edge E5
                               16,17,18, // E5 switches with E4
                               22,24,23, // F0 is uv plane, reverts normal
                               28,29,30, // F1 is uw plane, switches with F2
                               25,26,27, // F2 is vw plane, switches with F1
                               31,33,32};// F3 is uvw plane, reverts normal

class MTetrahedronN : public MTetrahedron {
 protected:
  std::vector<MVertex *> _vs;
  const char _order;
  double _disto;
 public:
  MTetrahedronN(MVertex *v0, MVertex *v1, MVertex *v2, MVertex *v3, 
                std::vector<MVertex*> &v, char order, int num=0, int part=0) 
    : MTetrahedron(v0, v1, v2, v3, num, part) , _vs (v), _order(order),_disto(-1.e22)
  {
    for(unsigned int i = 0; i < _vs.size(); i++) _vs[i]->setPolynomialOrder(_order);
  }
  MTetrahedronN(std::vector<MVertex*> &v, char order, int num=0, int part=0) 
    : MTetrahedron(v[0], v[1], v[2], v[3], num, part) , _order(order),_disto(-1.e22)
  {
    for(unsigned int i = 4; i < v.size(); i++) _vs.push_back(v[i]);
    for(unsigned int i = 0; i < _vs.size(); i++) _vs[i]->setPolynomialOrder(_order);
  }
  ~MTetrahedronN(){}
  virtual double distoShapeMeasure();
  virtual int getPolynomialOrder() const { return _order; }
  virtual int getNumVertices() const { return 4 + _vs.size(); }
  virtual MVertex *getVertex(int num){ return num < 4 ? _v[num] : _vs[num - 4]; }
  virtual int getNumEdgeVertices() const { return 6 * (_order - 1); }
  virtual int getNumFaceVertices() const
  {
    return 4 * ((_order - 1) * (_order - 2)) / 2;
  }
  virtual void getEdgeVertices(const int num, std::vector<MVertex*> &v) const
  {
    v.resize(_order + 1);
    MTetrahedron::_getEdgeVertices(num, v);
    int j = 2;
    const int ie = (num + 1) * (_order -1);
    for(int i = num * (_order -1); i != ie; ++i) v[j++] = _vs[i];
  }
  virtual void getFaceVertices(const int num, std::vector<MVertex*> &v) const
  {
    v.resize(3 + 3 * (_order - 1) + (_order-1) * (_order - 2) /2);
    MTetrahedron::_getFaceVertices(num, v);
    int j = 3;
    int nbV = (_order - 1) * (_order - 2) / 2;
    const int ie = (num+1)*nbV;
    for(int i = num*nbV; i != ie; ++i) v[j++] = _vs[i];
  }
  virtual int getNumVolumeVertices() const 
  {
    switch(getTypeForMSH()){
    case MSH_TET_35 : return 1;
    case MSH_TET_56 : return 4;
    default : return 0;
    }    
  }
  virtual int getTypeForMSH() const 
  {
    // (p+1)*(p+2)*(p+3)/6
    if(_order == 3 && _vs.size() + 4 == 20) return MSH_TET_20; 
    if(_order == 4 && _vs.size() + 4 == 34) return MSH_TET_34; 
    if(_order == 4 && _vs.size() + 4 == 35) return MSH_TET_35; 
    if(_order == 5 && _vs.size() + 4 == 56) return MSH_TET_56; 
    if(_order == 5 && _vs.size() + 4 == 52) return MSH_TET_52; 
    return 0;
  }
  virtual void revert() 
  {    
    MVertex *tmp;
    tmp = _v[1]; _v[1] = _v[2]; _v[2] = tmp;    
    switch (getTypeForMSH()) {
    case MSH_TET_20:
      {
        std::vector<MVertex*> inv(16);
        for (int i=0;i<16;i++) inv[i] = _vs[reverseTet20[i+4]-4];
        _vs = inv;
        break;
      }
    case MSH_TET_35:
      {
        std::vector<MVertex*> inv(31);
        for (int i=0;i<31;i++) inv[i] = _vs[reverseTet35[i+4]-4];
        _vs = inv;
        break;
      }
    case MSH_TET_34:
      {
        std::vector<MVertex*> inv(30);
        for (int i=0;i<30;i++) inv[i] = _vs[reverseTet34[i+4]-4];
        _vs = inv;
        break;
      }
    default:
      {
        Msg::Error("Reversion of %d order tetrahedron (type %d) not implemented\n",
                   _order, getTypeForMSH());
        break;
      }
    }
  }
  virtual void getEdgeRep(int num, double *x, double *y, double *z, SVector3 *n);
  virtual int getNumEdgesRep();
  virtual void getFaceRep(int num, double *x, double *y, double *z, SVector3 *n);
  virtual int getNumFacesRep();
};

#endif

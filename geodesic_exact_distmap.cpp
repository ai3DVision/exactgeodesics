/**
 *
 * Compile with: mex -v geodesic_exact_distmap.cpp
 * Indices are expected to be in cpp format (starting at 0).
 *
 */

#include "mex.h"
#include "geodesic_algorithm_exact.h"
#include "geodesic_error.h"
#include "geodesic_datastructures.h"
#include <iostream>

#include <cstring>
#include <string>

using namespace std;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
  // DEBUG
	ofstream fout("./debug.txt", ios::out);
	fout << nlhs << " " << nrhs << "\n";

// check and read mex input
  if (nrhs != 3 || nlhs != 1)
    mexErrMsgTxt("Usage: [distancemap] = geodesic_exact(vertices, triangles, source). Transform indices in triangles to initial 0.");

  // read vertices
  const double* const vertices = mxGetPr(prhs[0]);
  const long nVertices = long( mxGetN(prhs[0]) );
  const int dimVertices = int( mxGetM(prhs[0]) );

  // read triangles
  const double* const triangles = mxGetPr(prhs[1]);
  const long nTriangles = long( mxGetN(prhs[1]) );
  const int dimTriangles = int( mxGetM(prhs[1]) );

  // read source
  const double* s = mxGetPr(prhs[2]);

  // check dimensions
  if (dimTriangles != 3)
    mexErrMsgTxt("Triangles must be given in 3xm.");

  if (dimVertices != 3)
    mexErrMsgTxt("Vertices must be given in 3xn.");

    fout << "Check complete." << "\n";

// calculate geodesics
try
{
  geodesic::Mesh mesh_;
  geodesic::GeodesicAlgorithmExact* algorithm_;
  geodesicPoints* points_ = new geodesicPoints(vertices, int(nVertices));
  geodesicFaces* faces_ = new geodesicFaces(triangles, int(nTriangles));

  fout << "Declaration complete." << "\n";

  mesh_.initialize_mesh_data(*points_, *faces_);

  fout << "Mesh initialization complete." << "\n";

  algorithm_ = new geodesic::GeodesicAlgorithmExact(&mesh_);

  fout << "Algorithm initialization complete." << "\n";

  geodesic::SurfacePoint source(&mesh_.vertices()[ s[0] ]);
  std::vector<geodesic::SurfacePoint> all_sources(1, source);
  algorithm_->propagate(all_sources);

  fout << "Propagation complete." << "\n";

  // read out result
  plhs[0] = mxCreateDoubleMatrix(nVertices, 1, mxREAL);
  double* distmap = mxGetPr(plhs[0]);

  for(int i=0; i < nVertices; i++)
  {
    geodesic::SurfacePoint p(&mesh_.vertices()[i]);
    double distance;
    algorithm_->best_source(p, distance);
    distmap[i] = distance;
  }
} catch (std::exception& e)
  {
    const std::string msg = "Exception caught: " + std::string(e.what());
    mexErrMsgTxt(msg.c_str());
  }

}

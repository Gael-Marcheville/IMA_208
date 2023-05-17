//---------------------
// You mignt want to install CGAL properly before doing anything...
// But I don't remember how to, so good luck.
//
// TP done by Arnault VERRET
//---------------------


#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/squared_distance_3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_3<K> Delaunay;
typedef Delaunay::Point Point;
typedef Delaunay::Triangle Triangle;

// Creates a stl file from a set of triangles
void make_stl(const std::vector<Triangle>& triangles){
  // We know loop over the triangles in our map
  // and fill the STL file
  std::ofstream stlfile("out.stl");
  if (stlfile.is_open()){
    stlfile << "solid solid\n";
    for (Triangle t: triangles) {
      auto normal = CGAL::normal(
        t[0],
        t[1],
        t[2]
      );
      // normal calculation
      normal /= sqrt(CGAL::squared_distance(Point(normal.x(), normal.y(), normal.z()), Point(0, 0, 0)));

      // Write in the stl file
      stlfile << "facet normal " << normal.x() << " " << normal.y() << " " << normal.z() << "\n";
      stlfile << "    outer loop\n";
      stlfile << "        vertex " << t[0].x() << " " << t[0].y() << " " << t[0].z() << "\n";
      stlfile << "        vertex " << t[1].x() << " " << t[1].y() << " " << t[1].z() << "\n";
      stlfile << "        vertex " << t[2].x() << " " << t[2].y() << " " << t[2].z() << "\n";
      stlfile << "    endloop\n";
      stlfile << "endfacet\n";
    }
    stlfile.close();
  }
}

// Add triangles to a set if their circumcircle isn't too big
void add_triangles(Delaunay::Finite_facets_iterator& fit, std::vector<Triangle>& triangles, float alpha){
  // Take the vertices from the facet. Not as simple as it should be...
  std::pair<Delaunay::Cell_handle, int> facet = *fit;
  Triangle t = Triangle(
    facet.first->vertex((facet.second+1)%4)->point(),
    facet.first->vertex((facet.second+2)%4)->point(),
    facet.first->vertex((facet.second+3)%4)->point()
  );

  auto center_point = CGAL::circumcenter(t);
  // Check if the triangles is small enough
  if(CGAL::squared_distance(t[0], center_point) < alpha){
    triangles.push_back(t);
  }
}

// Compute the "best" alpha value
void get_alpha(Delaunay::Finite_cells_iterator& vit, float& alpha, float& global_weight){
}

int main(int argc, char **argv)
{
    std::cout << "Loading data" << std::endl;
    Delaunay T;
    const char* fname =argv[1]; //Reading the filename from the arguments
    float alpha{0};
    if (argc > 2){
      std::string alpha_s(argv[2]);
      alpha = 0.1;
    }

    std::ifstream stream(fname); //Reading the file
    Point p;
    while(!stream.eof()) //While the file is completely read
    {
        stream>>p; //Save line by line (x,y,z coordinates) to a point variable
        T.insert(Point(p.x(),p.y(),p.z())); //Insert the point into incremental Delaunay construction
    }

    if (argc < 3){
      Delaunay::Finite_cells_iterator vit; //An iterator variable that can iterate over the Delaunay cells (tetrahedrons)
      std::cout << "Finding Alpha" << std::endl;
      float global_weight = 0;
      // We loop over all cells because we want to take the tiniest triangle.
      for(vit=T.finite_cells_begin(); vit!=T.finite_cells_end(); vit++)
        get_alpha(vit, alpha, global_weight); // we compute alpha using all tetrahedrons
    }

    Delaunay::Finite_facets_iterator fit; //An iterator variable that can iterate over the Delaunay facets (triangles)

    std::cout << "Choosing triangles" << std::endl;
    std::vector<Triangle> triangles_to_stl{};
    for(fit=T.finite_facets_begin(); fit!=T.finite_facets_end(); fit++)
        add_triangles(fit, triangles_to_stl, alpha); // Add all triangle according to alpha

    std::cout << "Creating stl file" << std::endl;
    make_stl(triangles_to_stl);
    std::cout << "Done" << std::endl;
    return 1;
}
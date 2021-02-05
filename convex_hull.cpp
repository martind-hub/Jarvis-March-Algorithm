#include <iostream>
#include "read.points.h"
#include <string>
#include <list>
#include <vector>

// define a structure that has the properties of a point
struct point
{
  double x = {0.0};
  double y = {0.0};
  int z = {0};
};

// make the output look nice when printing points
std::ostream& operator<<(std::ostream& os, point p)
{
  os << "(" << p.x << "," << p.y << ")";
  return os;
}

// define a function to convert a list of vectors to a vector of points
std::vector<point> conversion_points(std::list<std::vector<double>>& input_list_vectors, std::vector<point> output_vector_points) {
  point p;
  std::vector<point>::iterator it_data {output_vector_points.begin()};
  for(auto item : input_list_vectors)
  {
    p.x = item[0];
    p.y = item[1];
    *it_data = p;
    advance(it_data, 1);
  }
  return output_vector_points;
}

// define a function to determine the left-most point in a set of points
point left_most_point(std::vector<point>& SET) {
  point p = SET[0];
  for (int i = {1}; i < SET.size(); ++i) {
    if (p.x > SET[i].x) {
      p = SET[i];
    }
  }
  return p;
}

// define a function to determine the right-most point in a set of points
point right_most_point(std::vector<point>& SET) {
  point p = SET[0];
  for (int i = {1}; i < SET.size(); ++i) {
    if (p.x < SET[i].x) {
      p = SET[i];
    }
  }
  return p;
}

// define a function to determine the top-most point in a set of points
point top_most_point(std::vector<point>& SET) {
  point p = SET[0];
  for (int i = {1}; i < SET.size(); ++i) {
    if (p.y < SET[i].y) {
      p = SET[i];
    }
  }
  return p;
}

// define a function to determine the bottom-most point in a set of points
point bottom_most_point(std::vector<point>& SET) {
  point p = SET[0];
  for (int i = {1}; i < SET.size(); ++i) {
    if (p.y > SET[i].y) {
      p = SET[i];
    }
  }
  return p;
}

// define a function to evaluate the cross product of two vectors
// and also determine the orientation of three points
double cross_product(point pivot, point a, point b) {
  double vector1x = a.x - pivot.x;
  double vector1y = a.y - pivot.y;
  double vector2x = b.x - pivot.x;
  double vector2y = b.y - pivot.y;
  double crossproduct = vector1x * vector2y - vector1y * vector2x;
  return crossproduct;
}

// define a function that extracts the convex hull with collinear points from a set of points
std::list<point> find_hull(std::list<point>& INCOMPLETE_HULL, std::vector<point>& SET, double& precision) {
  // counter to allow us to keep track of where we are while removing elements of a list
  int counter = 0;
  point p = left_most_point(SET), q, r;
  point px_min = p;
  // first add the left-most point to INCOMPLETE_HULL
  INCOMPLETE_HULL.push_back(px_min);
  auto it_incomplete_hull = INCOMPLETE_HULL.begin();
  INCOMPLETE_HULL.erase(it_incomplete_hull);

  while (1) {
    for (int i = {0}; i < SET.size(); ++i) {
      q = SET[i];
      //checks whether p and q are the same
      if (abs(q.x - p.x) < precision && abs(q.y - p.y) < precision) {continue;}
      for (int j = {0}; j < SET.size(); ++j) {
        r = SET[j];
        //checks whether r and p or r and q are the same
        if ((abs(r.x - p.x) < precision && abs(r.y - p.y) < precision) || (abs(r.x - q.x) < precision && abs(r.y - q.y) < precision)) {continue;}
        // negative cross product means q does not lie on the convex hull; add collinear points too
        if (cross_product(p, q, r) < 0 && abs(cross_product(p, q, r)) > precision) {goto START_AGAIN;}
      }
      // add point the correct point q to the hull
      INCOMPLETE_HULL.push_back(q);
      break;
      START_AGAIN:;
    }
    p = q;
    it_incomplete_hull = INCOMPLETE_HULL.begin();
    ++counter;
    advance(it_incomplete_hull, counter);
    if (abs(px_min.x - (*it_incomplete_hull).x) < precision && abs(px_min.y - (*it_incomplete_hull).y) < precision) {
      INCOMPLETE_HULL.erase(it_incomplete_hull);
      break;
    }
  }

  return INCOMPLETE_HULL;
}

// define a function that offsets collinear points to the z axis
// and returns the "edges" of the convex hull
std::list<point> remove_collinear(std::list<point> SET, point& offset, double precision) {
  // offset the collinear points
  auto it_hull = SET.begin();
  point p1 = *it_hull;
  advance(it_hull, 1);
  point p2 = *it_hull;
  advance(it_hull, 1);
  point p3 = *it_hull;
  int initial_number = SET.size();
  for (int i = 2; i < initial_number - 1; ++i) {
    it_hull = SET.begin();
    while (abs(cross_product(p1, p2, p3)) < precision) {
      p2 = p3;
      advance(it_hull, i-1);
      *it_hull = offset;
      advance(it_hull, 2);
      p3 = *it_hull;
      ++i;
      it_hull = SET.begin();
    }
    it_hull = SET.begin();
    p1 = p2;
    p2 = p3;
    advance(it_hull, i + 1);
    p3 = *it_hull;
  }

  // create a list of points to store only the edges
  point p0, p;
  std::list<point> COLLINEAR_REMOVED {p0};
  std::list<point>::iterator it_convex {COLLINEAR_REMOVED.begin()};
  for (auto item : SET) {
    if (item.z == offset.z) {
      continue;
    }
    p = item;
    *it_convex = p;
    COLLINEAR_REMOVED.push_back(p0);
    advance(it_convex, 1);
  }

  COLLINEAR_REMOVED.erase(it_convex);
  return COLLINEAR_REMOVED;
}

int main()
{
  point p0, offset_point = {0, 0, 5};
  // define precision, so that you can compare double values
  double epsilon = {0.0000001};
  // create a list of points that will store points on the boundary of the convex hull
  std::list<point> HULL {p0};
  // a list to store only the edges of the convex hull (i.e removed collinear points)
  std::list<point> CONVEX_HULL {p0};
  // import the data
  std::string filename {"./random.csv"};
  // read the data into a list of vectors
  std::list<std::vector<double>> xy_pairs {readpoints(filename)};

  // create a vector of points of length the size of the input data
  std::vector<point> DATA(xy_pairs.size(), p0);

  // convert the list of vectors to a vector of points
  DATA = conversion_points(xy_pairs, DATA);

  // establish trivial cases
  int i = 1;
  if (DATA.size() == 1) {
    std::cout << "You have provided only one point!" << std::endl;
    return 0;
  } else if (DATA.size() == 2) {
    std::cout << "You have provided only two points" << std::endl;
    std::cout << "The convex hull is the line segment joining them:" << std::endl;
    for (auto item : DATA) {
      std::cout << item << std::endl;
    }
    return 0;
  } else if (DATA.size() >= 3) {
      while (i <= (DATA.size() - 2)) {
        if (abs(cross_product(DATA[i-1], DATA[i], DATA[i + 1])) < epsilon) {
          ++i;
        } else {
          break;
        }
      }
      if ((i == (DATA.size() - 1)) && abs(cross_product(DATA[i - 2], DATA[i-1], DATA[i])) < epsilon) {
        std::cout << "All points are collinear" << std::endl;
        if (DATA[i-1].x != DATA[i].x) {
          std::cout << "The convex hull will be the line segment joining the points:" << std::endl;
          std::cout << left_most_point(DATA) << ", " << right_most_point(DATA) << std::endl;
          return 0;
        } else {
          std::cout << "The convex hull will be the line segment joining the points:" << std::endl;
          std::cout << bottom_most_point(DATA) << ", " << top_most_point(DATA) << std::endl;
          return 0;
        }
      }
  }

  // find the hull with collinear points
  HULL = find_hull(HULL, DATA, epsilon);

  // remove collinear points
  CONVEX_HULL = remove_collinear(HULL, offset_point, epsilon);

  // print the points in the convex hull
  for (auto points : CONVEX_HULL) {
    std::cout << points << std::endl;
  }

  return 0;
}

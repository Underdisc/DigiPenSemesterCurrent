/*****************************************************************************/
/*!
\file hull-bruteforce.cpp
\author Connor Deakin
\par E-mail: connortdeakin\@gmail.com
\par Project: CS 330 Assignment 3
\date 2017/11/02
\brief
  Contains the brute force implementations for finding convex hulls.
*/
/*****************************************************************************/

#include "hull-bruteforce.h"
#include <algorithm>
#include <iostream>

bool Point::operator==( Point const& arg2 ) const {
  return ( (x==arg2.x) && (y==arg2.y) );
}

std::ostream& operator<< (std::ostream& os, Point const& p) {
  os << "(" << p.x << " , " << p.y << ") ";
  return os;
}

std::istream& operator>> (std::istream& os, Point & p) {
  os >> p.x >> p.y;
  return os;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//return value is (on left, on right)
//right is indeed relative to the direction of the vector
std::pair<bool,bool> get_location(
  float const& p1x, //check which side of the line (p1x,p1y)-->(p2x,p2y)
  float const& p1y, //point (qx,qy) is on
  float const& p2x,
  float const& p2y,
  float const& qx,
  float const& qy)
{
  Point dir   = {p2x-p1x,p2y-p1y};
  Point norm  = {dir.y, -dir.x};
  Point point = {qx-p1x,qy-p1y};
  //scalar product is positive if on right side
  float scal_prod = norm.x*point.x + norm.y*point.y;
  return std::make_pair( (scal_prod<0), (scal_prod>0));
}

/*****************************************************************************/
/*!
\brief
  Returns a set containing all indicies of points that make a convex hull.

\param points
  The points that will be used to form the convex hull.

\return A set of the indicies for points that form the convex hull.
*/
/*****************************************************************************/
std::set<int> hullBruteForce ( std::vector< Point > const& points ) {
  int num_points = points.size();
  if ( num_points < 3 ) throw "bad number of points";
  std::set<int> hull_indices;
  // going through all pairs of points
  for(int p1_i = 0; p1_i < num_points; ++p1_i){
    for(int p2_i = p1_i + 1; p2_i < num_points; ++p2_i){
      // finding if all points are on one side
      bool on_left = false;
      bool on_right = false;
      for(int q_i = 0; q_i < num_points; ++q_i){
        const Point & p1 = points[p1_i];
        const Point & p2 = points[p2_i];
        const Point & q = points[q_i];
        std::pair <bool, bool> result;
        result = get_location(p1.x, p1.y, p2.x, p2.y, q.x, q.y);
        if(result.first)
          on_left = true;
        if(result.second)
          on_right = true;
        if(on_left && on_right)
          break;
      }
      // add points if all other points are on the other side
      if(!(on_left && on_right)){
        hull_indices.insert(p1_i);
        hull_indices.insert(p2_i);
      }
    }
  }
  return hull_indices;
}

/*****************************************************************************/
/*!
\brief
  Finds a convex hull for a given set of points. The idicies returned will
  be in a counter clockwise order starting with the point that has the
  smallest x value.

\param points
  The points that the convex hull will be formed from.


\return A vector containing the indicies of the points in the convex hull
  in a counter clockwise order.
*/
/*****************************************************************************/
std::vector<int> hullBruteForce2 ( std::vector< Point > const& points ) {
  int num_points = points.size();
  if ( num_points < 3 ) throw "bad number of points";
  std::vector<int> hull_indices;
  // find the point with the min x
  int start_p1_i = 0;
  for(int p_i = 1; p_i < num_points; ++p_i){
    if(points[p_i].x < points[start_p1_i].x)
      start_p1_i = p_i;
  }
  // adding the first point to the hull
  hull_indices.push_back(start_p1_i);
  // finding all other hull points
  int cur_p1_i = start_p1_i;
  for(int p2_i = 0; p2_i < num_points; ++p2_i){
    // skipping if we are at the same point
    if(p2_i == cur_p1_i)
      continue;
    // seeing if all points are on the left
    bool on_left = true;
    for(int q_i = 0; q_i < num_points; ++q_i){
      const Point & p1 = points[cur_p1_i];
      const Point & p2 = points[p2_i];
      const Point & q = points[q_i];
      std::pair<bool, bool> result;
      result = get_location(p1.x, p1.y, p2.x, p2.y, q.x, q.y);
      if(result.second){
        // a point is on the right
        on_left = false;
        break;
      }
    }
    // adding the point if all points were on left
    if(on_left){
      // check to see if we have arrived back at the first point
      if(p2_i == start_p1_i)
        break;
      hull_indices.push_back(p2_i);
      cur_p1_i = p2_i;
      // resetting out p2_i to go through all points again
      p2_i = 0;
    }
  }
  return hull_indices;
}

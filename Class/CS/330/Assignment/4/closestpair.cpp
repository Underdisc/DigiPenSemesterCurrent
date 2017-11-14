/*****************************************************************************/
/*!
\file closestPair.cpp
\author Connor Deakin
\par E-mail: connortdeakin\@gmail.com
\par Project: closestpair
\date 13/11/2017
\brief
  Contains the Divide and Conquer implementation of the closest pair problem.
*/
/*****************************************************************************/

#include "closestpair.h"
#include <algorithm>
#include <limits>
#include <cmath>
#include <iostream>
#include <utility>

std::ostream& operator<< (std::ostream& os, Point const& p) {
  os << "(" << p.x << " , " << p.y << ") ";
  return os;
}

std::istream& operator>> (std::istream& os, Point & p) {
  os >> p.x >> p.y;
  return os;
}

bool sortX(const Point & a, const Point & b)
{
  return (a.x < b.x);
}

bool sortY(const Point & a, const Point & b)
{
  return (a.y < b.y);
}

// HELPER FUNCTIONS ///////////////////////////////////////////////////////////

inline float min(float a, float b)
{
  return (a < b) ? a : b;
}

inline float squareDistance(const Point & a, const Point & b)
{
  float dx = a.x - b.x;
  float dy = a.y - b.y;
  return dx * dx + dy * dy;
}

inline float squareYDistance(const Point & a, const Point & b)
{
  float dy = a.y - b.y;
  return dy * dy;
}

inline float squareDistanceMinimum(const Point & a, const Point & b,
  const Point & c)
{
  float ab_dsquare = squareDistance(a, b);
  float ac_dsquare = squareDistance(a, c);
  float bc_dsquare = squareDistance(b, c);
  return min(min(ab_dsquare, ac_dsquare), bc_dsquare);
}

// closestPair ////////////////////////////////////////////////////////////////
/*****************************************************************************/
/*!
\brief
  Finds the minimum distance in a vector of points recursively.

\param P The vector of points.

\return The minimum distance in the vector of points.
*/
/*****************************************************************************/
float closestPairRecursive(const std::vector<Point> & P)
{
  int size = P.size();
  // base cases - find minimum distance
  if (size == 1)
    return std::numeric_limits<float>::max();
  if (size == 2)
    return sqrt(squareDistance(P[0], P[1]));
  if (size == 3)
    return sqrt(squareDistanceMinimum(P[0], P[1], P[2]));

  // copy into vectors for points on left and points on right
  std::vector<Point> Pleft;
  std::vector<Point> Pright;
  int mid = size / 2;
  for(int i = 0; i < mid; ++i)
    Pleft.push_back(P[i]);
  for(int i = mid; i < size; ++i)
    Pright.push_back(P[i]);
  // find the minimum distance on the left and right
  float d_left = closestPairRecursive(Pleft);
  float d_right = closestPairRecursive(Pright);
  float d_min = min(d_left, d_right);
  // find all points in center column
  float center = P[mid].x;
  float max_x = center + d_min;
  float min_x = center - d_min;
  // this could be optimized by starting at mid and going left and then right.
  // there is no need to check all of P since it is sorted.
  std::vector<Point> S;
  for(int i = 0; i < size; ++i){
    if(P[i].x > min_x && P[i].x < max_x)
      S.push_back(P[i]);
  }
  // see if there is a distance in the center column that is shorter
  float min_dsquare = d_min * d_min;
  std::sort(S.begin(), S.end(), sortY);
  int center_size = S.size();
  for(int i = 0; i < center_size - 1; ++i){
    int j = i + 1;
    // only check for a minimum distance while the y distance is less than
    // the minimum
    while(j < center_size && squareYDistance(S[j], S[i]) < min_dsquare){
      float dx = S[j].x - S[i].x;
      float dy = S[j].y - S[i].y;
      float dsquard = dx * dx + dy * dy;
      min_dsquare = min(dsquard, min_dsquare);
      ++j;
    }
  }
  return sqrt(min_dsquare);
}

/*****************************************************************************/
/*!
\brief
  Given a vector of points, the minimum distance between any two points in
  the set will be found.

\param points The vector of points.

\return The minimum distance.
*/
/*****************************************************************************/
float closestPair ( std::vector< Point > const& points ) {
  int size = points.size();
  if (size==0) throw "zero size subset";
  // sorting by x
  std::vector<Point> P(points);
  std::sort(P.begin(), P.end(), sortX);
  // finding the minimum distance
  return closestPairRecursive(P);
}

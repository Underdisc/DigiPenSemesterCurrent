/*****************************************************************************/
/*!
\file Water_dep.cpp
\author Connor Deakin
\par E-mail: connortdeakin\@gmail.com
\par Project: Water
\date 27/08/2017
\brief
  This contains all of the code that is not currently in use on the water
  project. Most of this code is functional, however, some issues with the
  results produced from this code caused put it here. This code may or may not
  be later revived from here if I can find a solution to the problems I am
  facing. The interface for the non-working code is in Water_dep.h
*/
/*****************************************************************************/

// static initializations
unsigned Water::Noise::m_NoisesCreated = 0;

///////////////////////////////////////////////////////////////////////////////
// Water Noise Implementation
///////////////////////////////////////////////////////////////////////////////
void Water::Noise::SetDirection(const glm::vec2 & direction)
{
  m_Direction = glm::normalize(direction);
  m_DirectionRadians = acos(m_Direction.x);
}

void Water::Noise::SetDirection(float radians)
{
  m_Direction.x = cos(radians);
  m_Direction.y = sin(radians);
  m_DirectionRadians = radians;
}

Water::Noise::Noise(const Water * water) :
  m_Amplitude(5.0f), m_Frequency(5.0f), m_Speed(0.5f), m_Water(water),
  m_Direction(1.0f, 0.0f)
{
  ++m_NoisesCreated;
  char initial_name[NAMEBUFFERSIZE];
  sprintf_s(initial_name, "Noise %d", m_NoisesCreated);
  m_Name = initial_name;
}

Water::Noise::Noise(float amplitude, float frequency, float speed,
  const Water * water, const glm::vec2 & direction) :
  m_Amplitude(amplitude), m_Frequency(frequency), m_Speed(speed),
  m_Water(water), m_Direction(direction)
{}

std::pair<float, glm::vec3> Water::Noise::Offset(float x, float y, float time) const
{
  glm::vec2 vert_position(x, y);
  glm::vec2 vert_delta_x(x + 0.5f, y);
  glm::vec2 vert_delta_y(x, y + 0.5f);

  ScalePosition(vert_position, time);
  ScalePosition(vert_delta_x, time);
  ScalePosition(vert_delta_y, time);

  float height(GenerateNoise(vert_position));
  float height_delta_x(GenerateNoise(vert_delta_x));
  float height_delta_y(GenerateNoise(vert_delta_y));

  glm::vec3 tangent(0.5f, height_delta_x - height, 0.0f);
  glm::vec3 bitangent(0.0f, height_delta_y - height, 0.5f);
  glm::vec3 normal(glm::cross(bitangent, tangent));

  return std::pair<float, glm::vec3>(height, normal);
}

inline float Water::Noise::GenerateNoise(const glm::vec2 & seed) const
{
  // int part of seed
  glm::vec2 i((float)((int)seed.x), (float)((int)seed.y));
  // fract part of seed
  glm::vec2 f(Fract(seed.x), Fract(seed.y));
  // the heights of the four corners of our square
  float a = Random(i);
  float b = Random(i + glm::vec2(1.0f, 0.0f));
  float c = Random(i + glm::vec2(0.0f, 1.0f));
  float d = Random(i + glm::vec2(1.0f, 1.0f));
  // u - interpolation value
  glm::vec2 u(2.0f * f);
  u.x = 3.0f - u.x;
  u.y = 3.0f - u.y;
  u = u * f * f;

  // constants that will never change
  float c1 = b - a;
  float c2 = c - a;
  float c3 = a - b - c + d;
  // interpolation of the four corners for vert height
  float h = m_Amplitude * (a + u.x*c1+ u.y*c2 + u.x*u.y*c3);

  /*
  float x = f.x;
  float xx = f.x * f.x;
  float xxx = f.x * f.x * f.x;
  float y = f.y;
  float yy = f.y * f.y;
  float yyy = f.y * f.y * f.y;

  //float x_slope = m_Amplitude * (c1 + u.y*c3);
  //float x_slope = m_Amplitude * ((6.0f * x - 6.0f * xx) * (c1 + c3 * u.y));
  float x_slope = m_Amplitude * (((6.0f*x) - (6.0f*xx))*c1 + ((18.0f*x*yy) - (18.0f*xx*yy) - (12.0f*x*yyy) + (12.0f*xx*yyy)));
  //float x_slope = m_Amplitude * (c1 * (6.0f*x - 2.0f*xx) + u.y * c3 * (6.0f*x - 2.0f*xx));
  //float x_slope = m_Amplitude * ((6.0f * x - 2.0f * xx) * (c1 + c3 * u.y));
  float y_slope = m_Amplitude * ((6.0f * y - 2.0f * yy) * (c2 + c3 * u.x));
  */
  return h;
}

void Water::Noise::ScalePosition(glm::vec2 & position, float time) const
{
  position.x = position.x / m_Water->m_XStride;
  // The y value is actually the z value.
  position.y = position.y / m_Water->m_ZStride;

  position *= m_Frequency;
  position.x += (m_Direction.x * time * m_Speed);
  position.y -= (m_Direction.y * time * m_Speed);

  if (position.x < 0)
    position.x *= -1;
  if (position.y < 0)
    position.y *= -1;
}

inline float Water::Noise::Random(const glm::vec2 & seed) const
{
  float random = sin(glm::dot(seed, glm::vec2(-0.400, 0.260)));
  random *= 54832.0;
  return Fract(random);
}

inline float Water::Noise::Fract(float value) const
{
  int int_value = (int)value;
  value -= (float)int_value;
  return value;
}

///////////////////////////////////////////////////////////////////////////////
// Ohter Water Functions
///////////////////////////////////////////////////////////////////////////////
void Water::ZeroVertexData()
{
  unsigned vertex_index = 0;
  for (unsigned z = 0; z < m_ZStride; ++z) {
    for (unsigned x = 0; x < m_XStride; ++x) {
      Vertex & vertex = m_VertexData[vertex_index];
      Vertex & normal = m_VertexData[vertex_index + m_NumVerts];
      vertex.m_X = x;
      vertex.m_Y = 0.0f;
      vertex.m_Z = z;
      normal.m_X = 0.0f;
      normal.m_Y = 0.0f;
      normal.m_Z = 0.0f;
      ++vertex_index;
    }
  }
}

void Water::UpdateSin()
{
  const float lamda = 5.0f;
  std::vector<float> x_heights;
  std::vector<float> z_heights;
  float time_passed = m_Stopwatch.TotalTime();

  for (unsigned i = 0; i < m_XStride; ++i) {
    float height = sin(sin(m_Stopwatch.TotalTime() * 1.5f) + m_VertexData[i].m_X + m_Stopwatch.TotalTime() * 4.0f);
    height += sin(0.5f * m_VertexData[i].m_X + m_Stopwatch.TotalTime());
    x_heights.push_back(height);
  }

  for (unsigned i = 0; i < m_ZStride; ++i) {
    unsigned z_index = i * m_XStride;
    float height = sin((m_VertexData[z_index].m_Z + m_Stopwatch.TotalTime() * 3.0f) / lamda) * 2.0f;
    z_heights.push_back(height);
  }

  unsigned size = m_VertexData.size();
  unsigned max_x_index = m_XStride - 1;
  unsigned max_z_index = m_ZStride - 1;
  for (unsigned i = 0; i < size; ++i) {
    // find vertex location
    unsigned x_index = i % m_XStride;
    unsigned z_index = i / m_ZStride;
    m_VertexData[i].m_Y = x_heights[x_index];// +z_heights[z_index];
  }
}

///////////////////////////////////////////////////////////////////////////////
// FlowField Implementation
///////////////////////////////////////////////////////////////////////////////
Water::FlowField::FlowField(unsigned x_stride, unsigned z_stride)
{
  for (unsigned i = 0; i < z_stride; ++i) {
    m_Masses.push_back(std::vector<float>());
    for (unsigned j = 0; j < x_stride; ++j) {
      m_Masses[i].push_back(0.0f);
    }
  }
}

void Water::FlowField::Update()
{
  unsigned z_stride = m_Masses.size();
  unsigned x_stride = m_Masses[0].size();
  for (unsigned iz = 0; iz < z_stride; ++iz) {
    for (unsigned ix = 0; ix < x_stride; ++ix) {
      UpdateCell(iz, ix);
    }
  }
}

inline void Water::FlowField::UpdateCell(unsigned z_index, unsigned x_index)
{
  float & cell_mass = m_Masses[z_index][x_index];

  float up_diff; bool up = false;
  float down_diff; bool down = false;
  float left_diff; bool left = false;
  float right_diff; bool right = false;



  unsigned z_max = m_Masses.size() - 1;
  unsigned x_max = m_Masses[0].size() - 1;

  if (z_index != 0) {
    up_diff = m_Masses[z_index - 1][x_index] - cell_mass;
    if (up_diff < 0)
      up = true;
  }
  if (z_index != z_max) {
    down_diff = m_Masses[z_index + 1][x_index] - cell_mass;
    if (down_diff < 0)
      down = true;
  }
  if (x_index != 0) {
    left_diff = m_Masses[z_index][x_index - 1] - cell_mass;
    if (left_diff < 0)
      left = true;
  }
  if (x_index != x_max) {
    right_diff = m_Masses[z_index][x_index + 1] - cell_mass;
    if (right_diff < 0)
      right = true;
  }

  float total_diff = 0.0f;
  if (up) total_diff += up_diff;
  if (down) total_diff += down_diff;
  if (left) total_diff += left_diff;
  if (right) total_diff += right_diff;

  float up_perc;
  float down_perc;
  float left_perc;
  float right_perc;

  if (up) up_perc = up_diff / total_diff;
  if (down) down_perc = down_diff / total_diff;
  if (left) left_perc = left_diff / total_diff;
  if (right) right_perc = right_diff / total_diff;

  float mass_to_move = 0.5f * -total_diff;
  cell_mass -= mass_to_move;

  if (up) m_Masses[z_index - 1][x_index] += mass_to_move * up_perc;
  if (down) m_Masses[z_index + 1][x_index] += mass_to_move * down_perc;
  if (left) m_Masses[z_index][x_index - 1] += mass_to_move * left_perc;
  if (right) m_Masses[z_index][x_index + 1] += mass_to_move * right_perc;

}

void Water::UpdateFlowField()
{
  m_FlowField.Update();
  unsigned size = m_VertexData.size();
  for (unsigned i = 0; i < size; ++i) {
    unsigned x_index = i % m_XStride;
    unsigned z_index = i / m_ZStride;
    m_VertexData[i].m_Y = m_FlowField.m_Masses[z_index][x_index];
  }
}

///////////////////////////////////////////////////////////////////////////////
// WaterEditor Noise Implementation
///////////////////////////////////////////////////////////////////////////////
void WaterEditor::Update()
{
  if (ImGui::Button("New Noise"))
    m_Water->m_Noises.push_back(Water::Noise(m_Water));
  for (Water::Noise & noise : m_Water->m_Noises)
    DisplayNoise(noise);
}

inline void WaterEditor::DisplayNoise(Water::Noise & noise)
{
  if (ImGui::TreeNode(noise.m_Name.c_str())) {
    ImGui::DragFloat("Amplitude", &noise.m_Amplitude, 0.01f);
    ImGui::DragFloat("Frequency", &noise.m_Frequency, 0.01f);
    ImGui::DragFloat("Speed", &noise.m_Speed, 0.01f);

    float new_direction = noise.m_DirectionRadians;
    ImGui::DragFloat("Direction", &new_direction, 0.01f);
    noise.SetDirection(new_direction);
    ImGui::TreePop();
  }
}

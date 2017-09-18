class Water
{
public:
  // Noise interface
  class Noise
  {
  public:
    void SetDirection(const glm::vec2 & direction);
    void SetDirection(float radians);
    std::string m_Name;
    float m_Amplitude;
    float m_Frequency;
    float m_Speed;
  private:
    Noise(const Water * water);
    Noise(float amplitude, float frequency, float speed, const Water * water,
      const glm::vec2 & direction);
    std::pair<float, glm::vec3> Offset(float x, float y, float time) const;
    float GenerateNoise(const glm::vec2 & position) const;
    void ScalePosition(glm::vec2 & position, float time) const;
    float Random(const glm::vec2 & seed) const;
    float Fract(float value) const;
    const Water * m_Water;
    glm::vec2 m_Direction;
    float m_DirectionRadians;
    static unsigned m_NoisesCreated;
    friend Water;
    friend WaterEditor;
  }
  // FlowField Interface
  class FlowField
  {
  public:
    FlowField(unsigned x_stride, unsigned z_stride);
    void Update();
    std::vector<std::vector<float> > m_Masses;
  private:
    void UpdateCell(unsigned z_index, unsigned x_index);
  };
private:
  // Other Water Functions
  void UpdateFlowField();
  void UpdateSin();
}

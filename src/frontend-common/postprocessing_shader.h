#pragma once
#include "common/rectangle.h"
#include "core/types.h"
#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace FrontendCommon {

class PostProcessingShader
{
public:
  enum : u32
  {
    PUSH_CONSTANT_SIZE_THRESHOLD = 128
  };

  enum class TextureFilter
  {
    Nearest,
    Linear
  };

  struct Option
  {
    enum : u32
    {
      MAX_VECTOR_COMPONENTS = 4
    };

    enum class Type
    {
      Invalid,
      Bool,
      Int,
      Float
    };

    union Value
    {
      s32 int_value;
      float float_value;
    };
    static_assert(sizeof(Value) == sizeof(u32));

    using ValueVector = std::array<Value, MAX_VECTOR_COMPONENTS>;
    static_assert(sizeof(ValueVector) == sizeof(u32) * MAX_VECTOR_COMPONENTS);

    std::string name;
    std::string ui_name;
    std::string dependent_option;
    Type type;
    u32 vector_size;
    ValueVector default_value;
    ValueVector min_value;
    ValueVector max_value;
    ValueVector step_value;
    ValueVector value;
  };

  struct Pass
  {
    std::string code;
    float output_scale = 1.0f;
    TextureFilter texture_filter = TextureFilter::Linear;
  };

  PostProcessingShader();
  PostProcessingShader(std::string name, std::string code);
  PostProcessingShader(const PostProcessingShader& copy);
  PostProcessingShader(PostProcessingShader& move);
  ~PostProcessingShader();

  PostProcessingShader& operator=(const PostProcessingShader& copy);
  PostProcessingShader& operator=(PostProcessingShader& move);

  ALWAYS_INLINE const std::string& GetName() const { return m_name; }
  ALWAYS_INLINE const std::string& GetCode(u32 pass) const { return m_passes[pass].code; }
  ALWAYS_INLINE float GetOutputScale(u32 pass) const { return m_passes[pass].output_scale; }
  ALWAYS_INLINE TextureFilter GetTextureFilter(u32 pass) const { return m_passes[pass].texture_filter; }
  ALWAYS_INLINE u32 GetNumPasses() const { return static_cast<u32>(m_passes.size()); }
  ALWAYS_INLINE const std::vector<Option>& GetOptions() const { return m_options; }
  ALWAYS_INLINE std::vector<Option>& GetOptions() { return m_options; }
  ALWAYS_INLINE bool HasOptions() const { return !m_options.empty(); }
  ALWAYS_INLINE bool IsLegacy() const { return m_is_legacy; }

  bool IsValid() const;

  const Option* GetOptionByName(const std::string_view& name) const;
  Option* GetOptionByName(const std::string_view& name);

  std::string GetConfigString() const;
  void SetConfigString(const std::string_view& str);

  bool LoadFromFile(std::string name, const char* filename);

  bool UsePushConstants() const;
  u32 GetUniformsSize() const;
  void FillUniformBuffer(void* buffer, u32 texture_width, s32 texture_height, s32 texture_view_x, s32 texture_view_y,
                         s32 texture_view_width, s32 texture_view_height, u32 window_width, u32 window_height,
                         float time) const;

private:
  struct CommonUniforms
  {
    float src_rect[4];
    float src_size[2];
    float resolution[2];
    float rcp_resolution[2];
    float window_resolution[2];
    float rcp_window_resolution[2];
    float time;
    float padding[1];
  };

  bool LoadFromLegacyFile(std::string name, const char* filename);
  bool LoadFromPassFile(std::string name, const char* filename);
  void LoadLegacyOptions();

  std::string m_name;
  std::vector<Pass> m_passes;
  std::vector<Option> m_options;
  bool m_is_legacy = false;
};

} // namespace FrontendCommon
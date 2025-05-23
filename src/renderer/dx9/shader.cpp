#include "shader.h"
#include "renderer.h"

#include <d3dcompiler.h>

namespace Impacto {
namespace DirectX9 {

static char const ShaderPath[] = "./shaders/dx9";
static char const FragShaderExtension[] = "_frag.hlsl";
static char const VertShaderExtension[] = "_vert.hlsl";

void Shader::Compile(char const* name, IDirect3DDevice9* device,
                     IDirect3DVertexDeclaration9* vertexDeclaration,
                     D3D_SHADER_MACRO* macros) {
  ImpLog(LogLevel::Debug, LogChannel::Render, "Compiling shader \"{:s}\"\n",
         name);

  VertexDeclaration = vertexDeclaration;

  ID3DBlob* vertexShaderBuffer{};
  ID3DBlob* errorBlob{};
  ID3DBlob* pixelShaderBuffer{};

  // Vertex shader
  std::string vertexShaderPath = fmt::format(FMT_COMPILE("{}/{}{}"), ShaderPath,
                                             name, VertShaderExtension);
  size_t sourceRawSz;
  char* source = (char*)SDL_LoadFile(vertexShaderPath.c_str(), &sourceRawSz);
  if (!source) {
    ImpLog(LogLevel::Debug, LogChannel::Render,
           "Failed to read shader source file\n");
    return;
  }
  auto result =
      D3DCompile(source, sourceRawSz, nullptr, macros, nullptr, "main",
                 "vs_3_0", 0, 0, &vertexShaderBuffer, &errorBlob);
  if (FAILED(result)) {
    ImpLog(LogLevel::Debug, LogChannel::Render,
           "Failed to compile shader source file {:s}\n",
           static_cast<const char*>(errorBlob->GetBufferPointer()));
    return;
  }

  result = device->CreateVertexShader(
      (DWORD*)vertexShaderBuffer->GetBufferPointer(), &VertexShader);
  if (FAILED(result)) return;

  // Pixel shader
  std::string fragShaderPath =
      fmt::format("{}/{}{}", ShaderPath, name, FragShaderExtension);
  source = (char*)SDL_LoadFile(fragShaderPath.c_str(), &sourceRawSz);
  if (!source) {
    ImpLog(LogLevel::Debug, LogChannel::Render,
           "Failed to read shader source file\n");
    return;
  }
  result = D3DCompile(source, sourceRawSz, nullptr, macros, nullptr, "main",
                      "ps_3_0", 0, 0, &pixelShaderBuffer, &errorBlob);
  if (FAILED(result)) {
    ImpLog(LogLevel::Debug, LogChannel::Render,
           "Failed to compile shader source file {:s}\n",
           static_cast<const char*>(errorBlob->GetBufferPointer()));
    return;
  }

  result = device->CreatePixelShader(
      (DWORD*)pixelShaderBuffer->GetBufferPointer(), &PixelShader);
  if (FAILED(result)) return;

  vertexShaderBuffer->Release();
  pixelShaderBuffer->Release();
  if (errorBlob) errorBlob->Release();
}

void Shader::UseShader(IDirect3DDevice9* device) {
  device->SetVertexDeclaration(VertexDeclaration);
  device->SetVertexShader(VertexShader);
  device->SetPixelShader(PixelShader);
}

}  // namespace DirectX9
}  // namespace Impacto
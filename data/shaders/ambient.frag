#version 330

in vec2 uv0;
in vec3 normal0;
in vec3 frag0;
in float vdist;

out vec4 color;

struct Material
{
  vec3 ambient;
  vec3 diffuse;
  sampler2D difftex;
  int colorkey;
};

struct DirLight
{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
};

struct PointLight
{
  vec3 position;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
};

uniform Material material;
uniform DirLight light;

#define POINT_LIGHT_COUNT 32
uniform PointLight pointLight[POINT_LIGHT_COUNT];

vec3 CalcDirLight(in DirLight light, in vec3 normal, in vec2 uv)
{
  vec3 ld = normalize(-light.direction);

  float diff = max(dot(normal, ld), 0.0);

  vec3 tex = vec3(texture(material.difftex, uv));
  vec3 ambient = light.ambient * tex;
  vec3 diffuse = light.diffuse * diff * tex;

  return(ambient + diffuse);
}

vec3 CalcPointLight(in PointLight light, in vec3 normal, in vec3 frag, in vec2 uv)
{
  vec3 ld = normalize(light.position - frag);

  float diff = max(dot(normal, ld), 0.0);

  float dist = length(light.position - frag);
  float atten = 1.0f / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

  vec3 tex = vec3(texture(material.difftex, uv));
  vec3 ambient = light.ambient * tex;
  vec3 diffuse = light.diffuse * diff * tex;

  ambient *= atten;
  diffuse *= atten;

  return(ambient + diffuse);
}

void main() {
  vec2 uv = uv0;
  //uv.x *= -1.0f;

  vec3 n = normalize(normal0);
  vec3 res = CalcDirLight(light, n, uv);

  for(int i = 0; i < POINT_LIGHT_COUNT; i++)
  {
    if(pointLight[i].diffuse.xyz != vec3(0,0,0))
      res += CalcPointLight(pointLight[i], n, frag0, uv);
  }

  float alpha = 1.0;

  vec3 fogCol = vec3(158.f, 186.f, 211.f);
  fogCol /= 255.f;
  
  res = mix(res, fogCol, vdist);

  if(material.colorkey == 1)
  {
    alpha = texture(material.difftex, uv).a;
  }

  color = vec4(res, alpha);
}

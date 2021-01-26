#define MAX_LIGHT_COUNT 10
#define DIRECTIONAL_LIGHT_TYPE 0
#define POINT_LIGHT_TYPE 1
#define SPOT_LIGHT_TYPE 2
#define TRUE 1
#define FASLE 0

struct PSIn
{
    float3 position : VO_POSITION;
    float3 normal : VO_NORMAL;
    float3 tangent : VO_TANGENT;
    float2 texCoord : VO_TEXCOORD;
    float4 s_position : SV_POSITION;
};

struct LightInfo
{
    float3 LightPosition;
    float  FalloffStartRadius;
    float3 LightIntensity;
    float  FalloffEndRadius;
    float3 LightDirection;
    float  FalloffStartAngle;
    float  FalloffEndAngle;
    uint   LightType;
    uint   LightEnbale;
};

cbuffer LightConstant : register(b2)
{
	LightInfo lights[MAX_LIGHT_COUNT];
}

float3 calcDirecionalLight(float3 originColor, float3 normal, int lightIndex);
float3 calcPointLight(float3 originColor, float3 pixelPos, float3 normal, int lightIndex);
float3 calcSpotLight(float3 originColor, float3 pixelPos, float3 normal, LightInfo light);

float4 main(PSIn pin) : SV_Target
{
    float3 originColor = float3(1.f, 1.f, 1.f);
    float3 finalColor = float3(0.f, 0.f, 0.f);
    for (int i = 0; i < MAX_LIGHT_COUNT; ++i)
    {
        switch (lights[i].LightType)
        {
            case DIRECTIONAL_LIGHT_TYPE:
                if(lights[i].LightEnbale == TRUE)
                    finalColor += calcDirecionalLight(originColor, normalize(pin.normal), i);
                break;
            case POINT_LIGHT_TYPE:
                if (lights[i].LightEnbale == TRUE)
                    finalColor += calcPointLight(originColor, pin.position, normalize(pin.normal), i);
                break;
            case SPOT_LIGHT_TYPE:
                if (lights[i].LightEnbale == TRUE)
                    finalColor += calcSpotLight(originColor, pin.position, normalize(pin.normal), lights[i]);
                break;
            default:
                break;
        }
    }
    return float4(finalColor, 1.f);
}

float3 calcDirecionalLight(float3 originColor, float3 normal, int lightIndex)
{
    return originColor * lights[lightIndex].LightIntensity * dot(normal, normalize(-lights[lightIndex].LightDirection));
}

float3 calcPointLight(float3 originColor, float3 pixelPos, float3 normal, int lightIndex)
{
    float3 lightDirection = normalize(lights[lightIndex].LightPosition - pixelPos);
    float3 intensity = clamp(lights[lightIndex].LightIntensity * dot(lightDirection, normal), 0.f, 1.f);
    float lightDistance = length(lights[lightIndex].LightPosition - pixelPos);
    if(lightDistance >= lights[lightIndex].FalloffEndRadius)
        return float3(0.f, 0.f, 0.f);
    else if(lightDistance < lights[lightIndex].FalloffStartRadius)
        return intensity;
    else
        return lerp(intensity, float3(0.f, 0.f, 0.f), (lightDistance - lights[lightIndex].FalloffStartRadius) / (lights[lightIndex].FalloffEndRadius - lights[lightIndex].FalloffStartRadius));
}

float3 calcSpotLight(float3 originColor, float3 pixelPos, float3 normal, LightInfo light)
{
    float lightDistance = length(light.LightPosition - pixelPos);
    float lightAttenu = 1.f;
    if(lightDistance > light.FalloffEndRadius)
    {
        lightAttenu = 0.f;
    }
    else if(lightDistance > light.FalloffStartRadius)
    {
        lightAttenu *= lerp(1.f, 0.f, (lightDistance - light.FalloffStartRadius) / (light.FalloffEndRadius - light.FalloffStartRadius));
    }
    
    float3 lightVec = normalize(light.LightPosition - pixelPos);
    float fallStartCos = cos(radians(light.FalloffStartAngle));
    float fallEndCos = cos(radians(light.FalloffEndAngle));
    float lightCos = dot(-lightVec, normalize(light.LightDirection));
    if(lightCos < fallEndCos)
        lightAttenu *= 0.f;
    else if(lightCos < fallStartCos)
        lightAttenu *= lerp(1.f, 0.f, (fallStartCos - lightCos) / (fallStartCos - fallEndCos));
    
    return originColor * clamp(dot(lightVec, normal), 0.f, 1.f) * light.LightIntensity * lightAttenu;
}
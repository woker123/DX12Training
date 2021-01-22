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
};

cbuffer LightConstant : register(b2)
{
    LightInfo lights[3];
}

float4 main(PSIn pin) : SV_Target
{
    float f = dot(normalize(pin.normal), normalize(-lights[0].LightDirection));
    return float4(f, f, f, 1);
}
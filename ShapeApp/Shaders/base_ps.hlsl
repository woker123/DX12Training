struct PSIn
{
    float3 position : VO_POSITION;
    float3 normal : VO_NORMAL;
    float3 tangent : VO_TANGENT;
    float2 texCoord : VO_TEXCOORD;
    float4 s_position : SV_POSITION;
};


float4 main(PSIn pin) : SV_Target
{
    return float4(pin.texCoord, 0, 1);
}
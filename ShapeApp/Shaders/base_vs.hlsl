struct VSInput
{
    float3 position : V_POSITION;
    float3 normal : V_NORMAL;
    float3 tangent : V_TANGENT;
    float2 texCoord : V_TEXCOORD;
};

struct VSOut
{
    float3 position : VO_POSITION;
    float3 normal : VO_NORMAL;
    float3 tangent : VO_TANGENT;
    float2 texCoord : VO_TEXCOORD;
    float4 s_position : SV_POSITION;
};

cbuffer PassBuffer : register(b0)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
}

cbuffer ObjectBuffer : register(b1)
{
    float4x4 gModel;
    float4x4 gInvModel;
}

VSOut main(VSInput vin)
{
    VSOut vout;
    float4x4 mvp = mul(gModel, gViewProj);
    vout.s_position = mul(float4(vin.position, 1), mvp);
    vout.normal = mul(float4(vin.normal, 0), transpose(gInvModel)).xyz;
    vout.tangent = mul(float4(vin.tangent, 0), transpose(gInvModel)).xyz;
    vout.texCoord = vin.texCoord;
    vout.position = mul(float4(vin.position, 1), gModel).xyz;
    
    return vout;
}

struct VSIn
{
    float3 position : POSITION;
    float3 color    : COLOR;
};

struct VSOut
{
    float4 s_position : SV_Position;
    float3 color : G_COLOR;
};

cbuffer MvpMatrix : register(b0)
{
    float4x4 mvpMatrix;
}

VSOut main(VSIn vin)
{
    VSOut vout;
    vout.s_position = mul(float4(vin.position, 1), mvpMatrix);
    vout.color = vin.color;
    
    return vout;
}
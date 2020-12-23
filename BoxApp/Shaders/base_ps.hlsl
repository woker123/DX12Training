struct PSIn
{
    float4 s_position : SV_Position;
    float3 color : G_COLOR;
};

float4 main(PSIn pin) : SV_TARGET
{
    return float4(pin.color, 1);
}
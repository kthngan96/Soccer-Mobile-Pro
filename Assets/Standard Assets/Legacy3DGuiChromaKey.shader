Shader "Hidden/SoccerGame3D/Legacy3DGuiChromaKey"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _KeyColor ("Key Color", Color) = (0, 0, 0, 1)
        _Tolerance ("Tolerance", Float) = 0.08
        _Softness ("Softness", Float) = 0.06
    }

    SubShader
    {
        Tags { "Queue" = "Overlay" "RenderType" = "Transparent" }
        Cull Off
        ZWrite Off
        ZTest Always
        Blend SrcAlpha OneMinusSrcAlpha

        Pass
        {
            CGPROGRAM
            #pragma vertex vert_img
            #pragma fragment frag
            #include "UnityCG.cginc"

            sampler2D _MainTex;
            fixed4 _KeyColor;
            float _Tolerance;
            float _Softness;

            fixed4 frag(v2f_img input) : SV_Target
            {
                fixed4 color = tex2D(_MainTex, input.uv);
                float keyDistance = distance(color.rgb, _KeyColor.rgb);
                color.a *= smoothstep(_Tolerance, _Tolerance + _Softness, keyDistance);
                return color;
            }
            ENDCG
        }
    }
}

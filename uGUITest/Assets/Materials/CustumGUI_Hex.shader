
Shader "CustumGUI/GUI_Hex" 
{
	Properties
	{
		_Aspect ("Aspect", Float) = 1.0
		_Color ("Tint", Color) = (1,1,1,1)

		_StencilComp ("Stencil Comparison", Float) = 8
		_Stencil ("Stencil ID", Float) = 0
		_StencilOp ("Stencil Operation", Float) = 0
		_StencilWriteMask ("Stencil Write Mask", Float) = 255
		_StencilReadMask ("Stencil Read Mask", Float) = 255

		_ColorMask ("Color Mask", Float) = 15

		_PatternFade ("Pattern Fade", Float) = 0.0
	}

	SubShader
	{
		Tags
		{ 
			"Queue"="Transparent" 
			"IgnoreProjector"="True" 
			"RenderType"="Transparent" 
			"PreviewType"="Plane"
			"CanUseSpriteAtlas"="True"
		}
		
		Stencil
		{
			Ref [_Stencil]
			Comp [_StencilComp]
			Pass [_StencilOp] 
			ReadMask [_StencilReadMask]
			WriteMask [_StencilWriteMask]
		}

		Cull Off
		Lighting Off
		ZWrite Off
		ZTest [unity_GUIZTestMode]
		Fog { Mode Off }
		Blend SrcAlpha OneMinusSrcAlpha
		ColorMask [_ColorMask]

		Pass
		{
		CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			#include "UnityCG.cginc"
			
			struct appdata_t
			{
				float4 vertex   : POSITION;
				float4 color    : COLOR;
				float2 texcoord : TEXCOORD0;
			};

			struct v2f
			{
				float4 vertex   : SV_POSITION;
				fixed4 color    : COLOR;
				half2 texcoord  : TEXCOORD0;
				half2 texcoord1  : TEXCOORD1;
			};

			fixed4 _Color;
			float _Aspect;
			float _PatternFade;

			v2f vert(appdata_t IN)
			{
				v2f OUT;
				OUT.vertex = mul(UNITY_MATRIX_MVP, IN.vertex);
				OUT.texcoord = IN.texcoord;
				OUT.texcoord1 = IN.vertex.xy;
#ifdef UNITY_HALF_TEXEL_OFFSET
				OUT.vertex.xy -= (_ScreenParams.zw-1.0);
#endif
				OUT.color = IN.color * _Color;
				return OUT;
			}

			sampler2D _MainTex;

			float  cmod(float  a, float  b) { return a - b * floor(a/b); }
			float2 cmod(float2 a, float2 b) { return a - b * floor(a/b); }
			float3 cmod(float3 a, float3 b) { return a - b * floor(a/b); }
			float4 cmod(float4 a, float4 b) { return a - b * floor(a/b); }

			float hex( float2 p, float2 h )
			{
				float2 q = abs(p);
				return max(q.x-h.y,max(q.x+q.y*0.57735,q.y*1.1547)-h.x);
			}

			fixed4 frag(v2f IN) : SV_Target
			{
				half4 color = tex2D(_MainTex, IN.texcoord) * IN.color;
				clip (color.a - 0.01);

				float scale = 100.0;
				float2 grid = float2(0.692, 0.4) * scale;
				float radius = 0.22 * scale;


				float2 pos = IN.texcoord1 * float2(_Aspect, 1.0) + float2(0.0, 0.3)*_Time.y*scale;
				float2 p1 = cmod(pos, grid) - grid*0.5;
				float2 p2 = cmod(pos+grid*0.5, grid) - grid*0.5;
				float d1 = hex(p1, radius);
				float d2 = hex(p2, radius);
				float d = min(d1, d2);
				float ditch = max((-cmod(pos.y+_Time.y*200.0, 500.0)+500.0) * 0.01 - 4.0, 0.0);

				float2 pi1 = floor((pos)/grid);
				float2 pi2 = floor((pos+grid*0.5)/grid);
				float pindex = d1 < d2 ? pi1.x*0.9+pi1.y*50.4 : pi2.x*1.2+pi2.y*60.3;

				if(color.a < 0.5) {
					if(d > 0.0) {
						color.a -= 0.2*(1.0-ditch*1.5);
						color.rgb *= ditch*5.0;
					}
					else if(d > -1.0) {
						color.rgb += 0.3;
					}
					color.a *= min(cmod(pos.y, 5.0)-1.0, 1.0);
				}

				float pf = max(1.0 + min(cmod(pindex, 1.0) - _PatternFade, 0.0) * 10.0f, 0.0) * min(color.a*10.0, 1.0);
				color.a *= pf;
				//color.rgb += (cmod(_Time.y, 0.033)*10.0);


				return color;
			}

		ENDCG
		}
	}
}

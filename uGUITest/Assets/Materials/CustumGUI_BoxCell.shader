
Shader "CustumGUI/GUI_BoxCell" 
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

			float2 pattern(float2 p)
			{
				p = frac(p);
				float r = 0.123;
				float v = 0.0, g = 0.0;
				r = frac(r * 9184.928);
				float cp, d;
				
				d = p.x;
				g += pow(clamp(1.0 - abs(d), 0.0, 1.0), 1000.0);
				d = p.y;
				g += pow(clamp(1.0 - abs(d), 0.0, 1.0), 1000.0);
				d = p.x - 1.0;
				g += pow(clamp(3.0 - abs(d), 0.0, 1.0), 1000.0);
				d = p.y - 1.0;
				g += pow(clamp(1.0 - abs(d), 0.0, 1.0), 10000.0);
				
				const int iter = 12;
				for(int i = 0; i < iter; i ++)
				{
					cp = 0.5 + (r - 0.5) * 0.9;
					d = p.x - cp;
					g += pow(clamp(1.0 - abs(d), 0.0, 1.0), 200.0);
					if(d > 0.0) {
						r = frac(r * 4829.013);
						p.x = (p.x - cp) / (1.0 - cp);
						v += 1.0;
					}
					else {
						r = frac(r * 1239.528);
						p.x = p.x / cp;
					}
					p = p.yx;
				}
				v /= float(iter);
				return float2(g, v);
			}

			fixed4 frag(v2f IN) : SV_Target
			{
				half4 color = tex2D(_MainTex, IN.texcoord) * IN.color;
				clip (color.a - 0.01);

				float scale = 0.002;
				float2 grid = float2(0.692, 0.4) * scale;
				float radius = 0.22 * scale;


				float2 pos = ((IN.texcoord1 * float2(_Aspect, 1.0)) + float2(0.0, 0.0)*_Time.y) * scale;
				float2 dg = pattern(pos);
				float d = dg.x;
				float a = clamp(1.0 - max(min(d, 2.0)-1.0, 0.0)*2.0, 0.0, 1.0);
				float ditch = (1.0-a) * max(0.5-cmod(pos.y-pos.x*0.3+(_Time.y*0.4)+(dg.y*0.4), 2.0), 0.0);

				if(color.a < 0.5) {
					color.a *= max(a, ditch*10.0);
					color.rgb += ditch * 2.0;
					//color.a *= min(cmod(pos.y, 5.0)-1.0, 1.0);
				}

				//color.a *= a;
				//color += ditch;
				//color.rgb += (cmod(_Time.y, 0.033)*10.0);


				return color;
			}

		ENDCG
		}
	}
}

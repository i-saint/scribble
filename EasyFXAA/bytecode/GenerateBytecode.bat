set fxc="%DXSDK_DIR%\Utilities\bin\x86\fxc"
set tostring=tostring
set cpp=ShaderSource.cpp

%fxc% CoverScreen.fx /Fo CoverScreen.fxo /T vs_4_0 /E VS
%fxc% Luma.fx /Fo Luma.fxo /T ps_4_0 /E PS
%fxc% FXAA.fx /Fo FXAA_Q12.fxo /T ps_4_0 /E PS /D FXAA_HLSL_4=1 /D FXAA_QUALITY__PRESET=12
%fxc% FXAA.fx /Fo FXAA_Q39.fxo /T ps_4_0 /E PS /D FXAA_HLSL_4=1 /D FXAA_QUALITY__PRESET=39
%tostring% CoverScreen.fxo g_VS_CoverScreen > %cpp%
%tostring% Luma.fxo g_PS_Luma >> %cpp%
%tostring% FXAA_Q12.fxo g_PS_FXAA_Q12 >> %cpp%
%tostring% FXAA_Q39.fxo g_PS_FXAA_Q39 >> %cpp%

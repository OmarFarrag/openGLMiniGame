#version 330 core

in vec2 vUV;
uniform sampler2D tex;
out vec4 outputColor;

const float offset = 1.0 / 200.0;  // This variable controls how many pixels we will take around the current fragment sample
// As the offset value decreases (the sample size decreases), more details get preserved during blurring

void main()
{

    float pix=900.0;
    float dx=15.0*(1.0/pix);
    float dy=10.0*(1.0/pix);
    vec2 pixalizationEquation= vec2(dx*floor(vUV.x/dx),dy * floor(vUV.y/dy));
    outputColor = texture(tex,pixalizationEquation); 
    // vec2 offsets[9] = vec2[](
    //     vec2(-offset,  offset), // top-left
    //     vec2( 0.0f,    offset), // top-center
    //     vec2( offset,  offset), // top-right
    //     vec2(-offset,  0.0f),   // center-left
    //     vec2( 0.0f,    0.0f),   // center-center
    //     vec2( offset,  0.0f),   // center-right
    //     vec2(-offset, -offset), // bottom-left
    //     vec2( 0.0f,   -offset), // bottom-center
    //     vec2( offset, -offset)  // bottom-right    
    // );

    // float blurringKernel[9] = float[](
    //     1.0 / 16, 2.0 / 16, 1.0 / 16,
    //     2.0 / 16, 4.0 / 16, 2.0 / 16,
    //     1.0 / 16, 2.0 / 16, 1.0 / 16 
    // );
    
    // vec3 sampleColor[9];
    // for(int i = 0; i < 9; i++)
    // {
    //     sampleColor[i] = vec3(texture(tex, vUV.st + offsets[i]));
    // }

    // vec3 fragColor = vec3(0.0);
    // for(int i = 0; i < 9; i++)
    //     fragColor += sampleColor[i] * blurringKernel[i];
    
    // outputColor = vec4(fragColor, 1.0);
}
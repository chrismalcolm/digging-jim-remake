uniform sampler2D texture;
uniform vec3 tint; // (hue, saturation, lightness) controls

// Convert RGB to HSL
vec3 rgb2hsl(vec3 c) {
    float maxC = max(c.r, max(c.g, c.b));
    float minC = min(c.r, min(c.g, c.b));
    float delta = maxC - minC;

    float h = 0.0;
    if (delta > 0.0) {
        if (maxC == c.r) {
            h = mod((c.g - c.b) / delta, 6.0);
        } else if (maxC == c.g) {
            h = ((c.b - c.r) / delta) + 2.0;
        } else {
            h = ((c.r - c.g) / delta) + 4.0;
        }
        h /= 6.0;
        if (h < 0.0) h += 1.0;
    }

    float l = (maxC + minC) * 0.5;

    float s = 0.0;
    if (delta > 0.0) {
        s = delta / (1.0 - abs(2.0 * l - 1.0));
    }

    return vec3(h, s, l);
}

// Convert HSL to RGB
vec3 hsl2rgb(vec3 c) {
    float h = c.x;
    float s = c.y;
    float l = c.z;

    float C = (1.0 - abs(2.0 * l - 1.0)) * s;
    float X = C * (1.0 - abs(mod(h * 6.0, 2.0) - 1.0));
    float m = l - C / 2.0;

    vec3 rgb = vec3(0.0);
    if (h < 1.0 / 6.0) rgb = vec3(C, X, 0.0);
    else if (h < 2.0 / 6.0) rgb = vec3(X, C, 0.0);
    else if (h < 3.0 / 6.0) rgb = vec3(0.0, C, X);
    else if (h < 4.0 / 6.0) rgb = vec3(0.0, X, C);
    else if (h < 5.0 / 6.0) rgb = vec3(X, 0.0, C);
    else rgb = vec3(C, 0.0, X);

    return rgb + vec3(m);
}

void main()
{
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    vec3 hsl = rgb2hsl(pixel.rgb);

    float hueLinear = mod(2.0 / 3.0 - hsl.x + 1.0, 1.0) * tint.x * 2.0;
    float lumLinear = hsl.z + (1.0 - hsl.z + hsl.y) * (tint.z - 0.5) / 3.0;
    float lumLimit = hsl.z + (1.0 - hsl.z) * (1.0 - hsl.y) * tint.z * 1.0 / 3.0;
    
    hsl.x = mod(2.0 / 3.0 - min(hueLinear, 1.0) + 1.0, 1.0);
    hsl.y = min(hsl.y * tint.y * 2.0, 1.0);
    hsl.z = min(max(lumLinear, 0.0), lumLimit);

    vec3 finalColor = hsl2rgb(hsl);
    gl_FragColor = vec4(finalColor, pixel.a);
}

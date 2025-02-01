const char* fragmentShaderSource = R"(
    #version 330 core
    #define MAX_LIGHTS 100

    struct sLights {
        vec3 position;      // position for point lights
        vec4 color;
        vec4 ambient;
        vec4 diffuse;       // diffuse color
        vec4 specular;      // specular color (w = specular power)
        vec4 atten;         // (constant, linear, quadratic, cutoff)
        vec4 direction;     // for directional lights/spot lights
        vec4 param1;        // x = light type, y = inner angle, z = outer angle
        vec4 param2;        // x = on/off
    };

    in vec3 FragPos;    // Incoming fragment position from vertex shader
    in vec3 FragNormal; // Incoming fragment normal from vertex shader
    in vec3 FragCol;

    uniform int numberOfLights; // Pass the number of active lights
    uniform sLights pLights[MAX_LIGHTS];  // Assuming you have a maximum of 10 lights
    uniform vec3 camLocation;   // Camera position

    out vec4 FragColor;

    // Calculating attenuation
    float CalculateAttenuation(sLights light, vec3 fragPos) {
        float distance = length(light.position - fragPos);
        
        if (distance > light.atten.w) {
            return 0.0;
        }
        
        return 1.0 / (light.atten.x + light.atten.y * distance + light.atten.z * (distance * distance));
    }
    
    // Calculating diffuse lighting
    vec3 CalculateDiffuse(sLights light, vec3 norm, vec3 lightDir) {
        float diff = max(dot(norm, lightDir), 0.0);
        return diff * light.color.rgb * light.diffuse.rgb;
    }

    // Calculating specular lighting
    vec3 CalculateSpecular(sLights light, vec3 norm, vec3 lightDir, vec3 viewDir) {
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), light.specular.w); // specular power in w component
        return spec * light.specular.rgb * light.color.rgb;
    }

    // Spotlight effect with penumbra (inner and outer cone angles)
    float CalculateSpotEffect(sLights light, vec3 fragPos) {
        vec3 lightDir = normalize(light.position - fragPos);
        float currentLightRayAngle = dot(lightDir, normalize(-light.direction.xyz));
        float outerConeAngleCos = cos(radians(light.param1.z));
        float innerConeAngleCos = cos(radians(light.param1.y));

        // Completely outside the cone
        if (currentLightRayAngle < outerConeAngleCos) {
            return 0.0;
        }
        // Inside the penumbra
        else if (currentLightRayAngle < innerConeAngleCos) {
            return (currentLightRayAngle - outerConeAngleCos) / (innerConeAngleCos - outerConeAngleCos);
        }
        // Fully inside the inner cone
        return 1.0;
    }
    
    void main() {
        vec3 norm = normalize(FragNormal);
        vec3 result = vec3(0.0);

        for (int i = 0; i < numberOfLights; i++) {

            vec3 lightDir;
            if (pLights[i].param1.x == 0.0) { // Point light
                lightDir = normalize(pLights[i].position - FragPos);
            } else if (pLights[i].param1.x == 1.0) { // Directional light
                lightDir = normalize(-pLights[i].direction.xyz);
            }

            float attenuation = CalculateAttenuation(pLights[i], FragPos);
            vec3 diffuse = CalculateDiffuse(pLights[i], norm, lightDir);
            vec3 viewDir = normalize(camLocation - FragPos);
            vec3 specular = CalculateSpecular(pLights[i], norm, lightDir, viewDir);

            // Spotlight effect (only applies if it's a spotlight)
            float spotEffect = 1.0;
            if (pLights[i].param1.x == 1.0) { // Spotlight
                spotEffect = CalculateSpotEffect(pLights[i], FragPos);
            }

            result += (diffuse * spotEffect + specular * spotEffect) * attenuation;
        }

        FragColor = vec4(result * FragCol, 1.0);
    }
)";

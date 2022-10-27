vec3 DirLight(Light lig, vec3 normal, vec3 fragposition,vec3 viewDir)
{
    vec3 lightDir = normalize(-lig.direction);
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 reflectDirection = 2*dot(norm, lightDir)*norm - lightDir;
    
    vec3 specular =vec3(0.0);
    float spec = pow(max(dot(viewDir, reflectDirection), 0.0), material.shininess*128);
    specular= lig.specular * spec * material.specular;

    vec3 ambient  = lig.ambient * material.ambient;
    vec3 diffuse  = lig.diffuse * diff * material.diffuse;
    return (ambient + diffuse + specular);
}
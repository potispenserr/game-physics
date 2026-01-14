#version 430 core
out vec4 FragColor;
uniform vec4 rayColor;
void main()
{
 
    FragColor = vec4(rayColor); 
}
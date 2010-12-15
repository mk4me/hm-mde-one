attribute vec4 weight;
attribute vec4 index;
attribute float numBones;

varying vec4 pos;
varying vec4 rawpos;

uniform mat4 boneMatrices[33];
uniform mat4 boneSpaceMatrices[33];
uniform vec4 color;
uniform vec4 lightPos;


varying vec3 normal;
varying vec3 viewvec;



void main(void)
{
    gl_Position = vec4 (0, 0, 0, 0);
    ivec4 ibonesidx = ivec4(index);
    for (int i = 0; i < 4; i++)
    {
        gl_Position += boneMatrices[int(index[i])] * boneSpaceMatrices[int(index[i])] * gl_Vertex * weight[i];
    }

    gl_Position = gl_ModelViewProjectionMatrix * gl_Position;

    normal = gl_NormalMatrix * gl_Normal;
    viewvec = gl_NormalMatrix * vec3 (gl_Vertex);
	
	// jesli chcemy zobaczyc graficznie ilu kosciom podlega vertex
	// gl_FrontColor = vec4(weight.x, weight.y, weight.z, weight.w);
	
	
	 //  Tymczasowo niech bêdzie tutaj - potem to wywale -  shader z lighitng.vert/frag
	 pos = gl_ModelViewMatrix * gl_Vertex;
	 rawpos = gl_Vertex;
	 gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
  
     color = gl_FrontMaterial.diffuse;
	 vec4 matspec = gl_FrontMaterial.specular;
	 float shininess = gl_FrontMaterial.shininess;
	 vec4 lightspec = gl_LightSource[0].specular;
	 vec4 lpos = gl_LightSource[0].position;
	 vec4 s = -normalize(pos-lpos);

     vec3 light = s.xyz;
     vec3 n = normalize(normal);
	 vec3 r = -reflect(light, n);
	 r = normalize(r);
	 vec3 v = -pos.xyz;
	 v = normalize(v);
   
	 vec4 diffuse  = color * max(0.0, dot(n, s.xyz)) * gl_LightSource[0].diffuse;
	 vec4 specular;
	 if (shininess != 0.0) 
	 {
		specular = lightspec * matspec * pow(max(0.0, dot(r, v)), shininess);
	 } else 
	 {
		specular = vec4(0.0, 0.0, 0.0, 0.0);
     }

	 gl_FrontColor = diffuse + specular;
}


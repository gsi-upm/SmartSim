
<<<<<<< .working
<<<<<<< .working
<<<<<<< .working
<<<<<<< .working
// uniform vec3 diffuseColor = vec3(255.f,252.f,181.f)/255.f*vec3(0.8,0.8,0.8)*0.8;
// uniform vec3 specularColor = vec3(101.f, 101.f, 101.f)/255.f ;
// uniform vec3 ambient = (vec3(255 + 127, 241, 0 + 2)/255.f)*(vec3(0.2,0.2,0.2));
// uniform float bWire = 1.f;
=======
uniform vec3 diffuseColor = vec3(255.f,252.f,181.f)/255.f*vec3(0.8,0.8,0.8)*0.8;
uniform vec3 specularColor = vec3(101.f, 101.f, 101.f)/255.f ;
uniform vec3 ambient = (vec3(255 + 127, 241, 0 + 2)/255.f)*(vec3(0.2,0.2,0.2));
varying vec3 normal,lightDir,halfVector;
>>>>>>> .merge-right.r1595
=======
uniform vec3 diffuseColor = vec3(1,1,1)*0.8;//vec3(255.f,252.f,181.f)/255.f*vec3(0.8,0.8,0.8)*0.8;
uniform vec3 specularColor = vec3(101.f, 101.f, 101.f)/255.f ;
uniform vec3 ambient = vec3(0,0,0);//(vec3(255 + 127, 241, 0 + 2)/255.f)*(vec3(0.2,0.2,0.2));
=======
const vec3 diffuseColor = vec3(1,1,1)*0.8;//vec3(255.f,252.f,181.f)/255.f*vec3(0.8,0.8,0.8)*0.8;
=======
const vec3 diffuseColor = vec3(1,1,1)*0.6;//vec3(255.f,252.f,181.f)/255.f*vec3(0.8,0.8,0.8)*0.8;
>>>>>>> .merge-right.r1678
const vec3 specularColor = vec3(101.0/255.0, 101.0/255.0, 101.0/255.0);
<<<<<<< .working
const vec3 ambient = vec3(0,0,0);//(vec3(255 + 127, 241, 0 + 2)/255.f)*(vec3(0.2,0.2,0.2));
>>>>>>> .merge-right.r1633
varying vec3 normal,lightDir,halfVector;
=======
const vec3 ambient = vec3(0.0,0.0,0.0);//(vec3(255 + 127, 241, 0 + 2)/255.f)*(vec3(0.2,0.2,0.2));
varying vec3 normal,lightDir[2],halfVector[2];
varying float dist[2];
>>>>>>> .merge-right.r1678
>>>>>>> .merge-right.r1630

void main (void)
{  
	vec3 n,halfV;
	float NdotL,NdotHV;
	/* The ambient term will always be present */

<<<<<<< .working
<<<<<<< .working
<<<<<<< .working
//    vec3 E = normalize(eyeVec);
//    vec3 R = reflect(-L, N);
//    float specular = pow( max(dot(R, E), 0.0),shine);
//    if (bWire)
// 	   gl_FragColor.rgb = 0;
//    else
   gl_FragColor = vec4(lambert,lambert,lambert,1.f) ; //vec4(diffuseColor*lambertTerm,1.f);// + vec4(specularColor*specular,1.f);//texel*texel;//textureRect(Transform, coord).x;// + texture2D(HahaTest, coord).x;//vec4(1.0,0.0,0.0,1.0);	   
=======
	vec4 color = vec4(ambient,1.0);
	/* a fragment shader can't write a varying variable, hence we need

	a new variable to store the normalized interpolated normal */
	n = normalize(normal);
	/* compute the dot product between normal and ldir */

	NdotL = max(dot(n,lightDir),0.0);
   if (NdotL > 0.0) {
		color += vec4(diffuseColor*NdotL,0);
		halfV = normalize(halfVector);
		NdotHV = max(dot(n,halfV),0.0);
		color += vec4(specularColor*pow(NdotHV, 30),0);
	}
   gl_FragColor = color;//vec4(diffuseColor*lambertTerm,1.f);// + vec4(specularColor*specular,1.f);//texel*texel;//textureRect(Transform, coord).x;// + texture2D(HahaTest, coord).x;//vec4(1.0,0.0,0.0,1.0);	   
>>>>>>> .merge-right.r1595
=======
=======
	float att;
>>>>>>> .merge-right.r1678
	vec4 color = vec4(ambient,1.0);
	/* a fragment shader can't write a varying variable, hence we need

	a new variable to store the normalized interpolated normal */
	n = normalize(normal);
	/* compute the dot product between normal and ldir */
	for (int i=0;i<2;i++)
	{
	    att = 1.0/(gl_LightSource[i].constantAttenuation + gl_LightSource[i].linearAttenuation * dist[i] + gl_LightSource[i].quadraticAttenuation * dist[i] * dist[i]);	
		NdotL = max(dot(n,lightDir[i]),0.0);
		if (NdotL > 0.0) {
		color += vec4(diffuseColor*NdotL,0)*att;
		halfV = normalize(halfVector[i]);
		NdotHV = max(dot(n,halfV),0.0);
		color += vec4(specularColor*pow(NdotHV, 30.0),0)*att;			
	}   
	}
<<<<<<< .working
   gl_FragColor = color;//vec4(diffuseColor*lambertTerm,1.f);// + vec4(specularColor*specular,1.f);//texel*texel;//textureRect(Transform, coord).x;// + texture2D(HahaTest, coord).x;//vec4(1.0,0.0,0.0,1.0);	   
>>>>>>> .merge-right.r1630
   //gl_FragColor.xyz = normal;
=======
    gl_FragColor = color;//vec4(diffuseColor*lambertTerm,1.f);// + vec4(specularColor*specular,1.f);//texel*texel;//textureRect(Transform, coord).x;// + texture2D(HahaTest, coord).x;//vec4(1.0,0.0,0.0,1.0);	   
    //gl_FragColor.xyz = normal;
>>>>>>> .merge-right.r1678
}

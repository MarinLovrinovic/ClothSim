#version 330 core
//postoje dodaci za sintaksno bojanje i parsiranje ovog koda, obavezno ih instalirajte za lakse programiranje. 
//Instalirajte RenderDoc i obavezno pogledajte kako se na grafickoj kartici izvrsava ovaj jednostavni program.

//prilikom jednog poziva iscrtavanja bit ce pokrenuto onoliko instanci ovog programa koliko ima vrhova za iscrtati!
layout (location = 0) in vec3 aPos;  //ovako definirana varijabla ce svakom sjencaru biti razlicita ovisno o koristenom VBO i glVertexAttribPointer
//ovako se povezuju buffer podaci iz glavne memorije i memorije na graf. kartici (indexi mora odgovarati):
	//		 layout (location = ---> 0 <---) in vec3 aPos;
	//	  glVertexAttribPointer(---> 0 <---, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(---> 0 <---);

//uniformna varijabla je jednaka svim sjencarima pri jednom pozivu iscrtavanja
uniform vec3 u_color; 
//ovako se povezuju uniformni podaci iz glavne memorije i memorije na graf. kartici:
		//																	 uniform vec3 ---> u_color <---; 
		//	GLint --->lokacijaUniformVarijable<--- = glGetUniformLocation(sjencar[0]->ID, ---> "u_color" <---);
	//glUniform3f(--->lokacijaUniformVarijable<---, 0.5, 1.0, 1.0);


out vec3 color; //izlaz iz sjencara je boja vrha koja se prilikom rasterizacije interpolira po cijelom trokutu. Pogledati shader0.frag

void main()
{
	color = u_color;
    gl_Position = vec4(aPos, 1.0); //uvijek se mora definirati i pozicija vrha
}
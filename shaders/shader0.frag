#version 330 core
//prilikom jednog poziva iscrtavanja bit ce pokrenuto onoliko instanci ovog programa koliko ima fragmenata (pixela) za iscrtati za sve poslane poligone!
out vec4 FragColor;  //izlaz iz sjencara fragmenata je boja tipa vec4(red, green, blue, alpha)

in vec3 color; //ulaz u sjencar fragmenata je izlaz iz sjencara vrhova u obliku vec3(red, green, blue)


void main()
{
    FragColor = vec4(color, 0.0);  //samo zapisemo boju u framebuffer (mjesto u memoriji koje je povezano s monitorom za iscrtavanje)
} 
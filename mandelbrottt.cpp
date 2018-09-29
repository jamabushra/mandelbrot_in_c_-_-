#include <include/SFML/Graphics.hpp>
#include <cmath>

const int IMAGE_WIDTH = 1000;
const int IMAGE_HEIGHT = 600;
double zoom = 0.004;   // allow the user to zoom in and out...
double offsetX = -0.7; // and move around
double offsetY = 0.0;
const int MAX = 127;
// maximum number of iterations for mandelbrot()
// don't increase MAX or the colouring will look strange

int mandelbrot(double, double, int);
sf::Color getColor(int);

int main()
{
    sf::RenderWindow window(sf::VideoMode(IMAGE_WIDTH, IMAGE_HEIGHT), "Mandelbrot");
    window.setFramerateLimit(30);

    sf::Image image;
    image.create(IMAGE_WIDTH, IMAGE_HEIGHT, sf::Color(0, 0, 0));
    sf::Texture texture;
    sf::Sprite sprite;

    bool stateChanged = true; // track whether the image needs to be regenerated

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                stateChanged = true; // image needs to be recreated when the user changes zoom or offset
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                    window.close();
                    break;
                case sf::Keyboard::Equal:
                    zoom *= 0.9;
                    break;
                case sf::Keyboard::Dash:
                    zoom /= 0.9;
                    break;
                case sf::Keyboard::W:
                    offsetY -= 40 * zoom;
                    break;
                case sf::Keyboard::S:
                    offsetY += 40 * zoom;
                    break;
                case sf::Keyboard::A:
                    offsetX -= 40 * zoom;
                    break;
                case sf::Keyboard::D:
                    offsetX += 40 * zoom;
                    break;
                default:
                    break;
                }
            default:
                break;
            }
        }

        if (stateChanged)
        { // only generate a new image if something has changed, to avoid unnecessary lag
            for (int x = 0; x < IMAGE_WIDTH; x++)
            {
                for (int y = 0; y < IMAGE_HEIGHT; y++)
                {
                    // convert x and y to the appropriate complex number
                    double real = (x - IMAGE_WIDTH / 2.0) * zoom + offsetX;
                    double imag = (y - IMAGE_HEIGHT / 2.0) * zoom + offsetY;
                    int value = mandelbrot(real, imag, MAX);
                    image.setPixel(x, y, getColor(value));
                }
            }
            texture.loadFromImage(image);
            sprite.setTexture(texture);
        }

        window.clear();
        window.draw(sprite);
        window.display();

        stateChanged = false;
    }

    return 0;
}

int mandelbrot(double startReal, double startImag, int maximum)
{
    int counter = 0;
    double zReal = startReal;
    double zImag = startImag;
    double nextRe;

    while (pow(zReal, 2.0) + pow(zImag, 2.0) <= 4.0 && counter <= maximum)
    {
        nextRe = pow(zReal, 2.0) - pow(zImag, 2.0) + startReal;
        zImag = 2.0 * zReal * zImag + startImag;
        zReal = nextRe;
        if (zReal == startReal && zImag == startImag)
        {              // a repetition indicates that the point is in the Mandelbrot set
            return -1; // points in the Mandelbrot set are represented by a return value of -1
        }
        counter += 1;
    }

    if (counter >= maximum)
    {
        return -1; // -1 is used here to indicate that the point lies within the Mandelbrot set
    }
    else
    {
        return counter; // returning the number of iterations allows for colouring
    }
}

sf::Color getColor(int iterations)
{
    int r, g, b;

    if (iterations == -1)
    {
        r = 0;
        g = 0;
        b = 0;
    }
    else if (iterations == 0)
    {
        r = 255;
        g = 0;
        b = 0;
    }
    else
    {
        // colour gradient:      Red -> Blue -> Green -> Red -> Black
        // corresponding values:  0  ->  16  ->  32   -> 64  ->  127 (or -1)
        if (iterations < 16)
        {
            r = 16 * (16 - iterations);
            g = 0;
            b = 16 * iterations - 1;
        }
        else if (iterations < 32)
        {
            r = 0;
            g = 16 * (iterations - 16);
            b = 16 * (32 - iterations) - 1;
        }
        else if (iterations < 64)
        {
            r = 8 * (iterations - 32);
            g = 8 * (64 - iterations) - 1;
            b = 0;
        }
        else
        { // range is 64 - 127
            r = 255 - (iterations - 64) * 4;
            g = 0;
            b = 0;
        }
    }

    return sf::Color(r, g, b);
}
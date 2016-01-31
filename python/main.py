import pygame
pygame.init()


def findPeaks(colortable, n):
    i = 0
    colorcmp = []
    colorpeaks = []

    while True:
        # Find highest color count (Very inefficient, but Python dicts can't be sorted).
        a = reduce(lambda a, x: a if x[1] < a[1] else x, colortable.iteritems(), (None, None))
        colortable[a[0]] = None  # Overwrite so it won't be picked again

        # TODO: store RGB in seperate values
        # Unpacking colors
        r = a[0] / 1000000
        g = a[0] / 1000 % 1000
        b = a[0] % 1000

        for c in colorcmp:
            if c is not None and abs(r+g+b - c) < 135:
                break
        else:
            colorcmp.append(r+g+b)
            colorpeaks.append(a[0])
            i += 1

        if i == n:
            break

    return colorpeaks


def processPNG(filename):
    surface = pygame.image.load(filename)
    colortable = {}
    for x in range(surface.get_width()):
        for y in range(surface.get_height()):
            r, g, b, a = surface.get_at((x, y))
            key = r*1000000+g*1000+b  # single int formated as RRRGGGBBB
            if colortable.get(key) is None:
                colortable[key] = 1
            else:
                colortable[key] += 1

    colorpeaks = findPeaks(colortable, 3)

    for c in colorpeaks:
        r = c / 1000000
        g = c / 1000 % 1000
        b = c % 1000
        print r, g, b


if __name__ == "__main__":
    processPNG("../pic.png")

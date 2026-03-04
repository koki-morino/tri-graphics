#ifndef SOURCES_H
#define SOURCES_H

#ifdef __cplusplus
extern "C" {
#endif

const char *vertexSource =
    "#version 100\n"

    "attribute vec2 position;\n"
    "attribute vec2 aTexCoord;\n"
    "uniform vec2 resolution;\n"
    "uniform vec2 translation;\n"
    "varying vec2 texCoord;\n"

    "void main()\n"
    "{\n"
    "    /* Move position */"
    "    vec2 translated = position + translation;\n"

    "    /* Pixel to clipspace */"
    "    vec2 clipspace = (translated / resolution) * 2.0 - 1.0;\n"

    "     /* Flip Y axis to make (0, 0) top-left */"
    "    gl_Position = vec4(clipspace * vec2(1, -1), 0, 1);\n"
    "    texCoord = aTexCoord;\n"
    "}\n";

const char *fragmentSource =
    "#version 100\n"

    "precision mediump float;\n"

    "varying vec2 texCoord;\n"
    "uniform sampler2D texture;\n"

    "void main()\n"
    "{\n"
    "    gl_FragColor = texture2D(texture, texCoord);\n"
    "}\n";

#ifdef __cplusplus
}
#endif

#endif // SOURCES_H

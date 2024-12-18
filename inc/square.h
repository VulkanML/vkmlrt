/*
#version 460


#extension GL_EXT_shader_explicit_arithmetic_types_int8 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int32 : require

layout(binding=0) buffer InputA { uint8_t x[]; } i;
layout(binding=1) buffer Output { uint8_t x[]; } o;

void main()
{
	o.x[gl_GlobalInvocationID.x] = i.x[gl_GlobalInvocationID.x] * i.x[gl_GlobalInvocationID.x];    
}
*/

static const unsigned char square[1156] =
{
    0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x0b, 0x00, 0x08, 0x00, 0x24, 0x00, 0x00, 0x00,  // ..#.........$...
    0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00,  // ................
    0x27, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x61, 0x11, 0x00, 0x00, 0x0a, 0x00, 0x07, 0x00,  // '.......a.......
    0x53, 0x50, 0x56, 0x5f, 0x4b, 0x48, 0x52, 0x5f, 0x38, 0x62, 0x69, 0x74, 0x5f, 0x73, 0x74, 0x6f,  // SPV_KHR_8bit_sto
    0x72, 0x61, 0x67, 0x65, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00, 0x01, 0x00, 0x00, 0x00,  // rage............
    0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, 0x30, 0x00, 0x00, 0x00, 0x00,  // GLSL.std.450....
    0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x06, 0x00,  // ................
    0x05, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00,  // ........main....
    0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x06, 0x00, 0x04, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00,  // ................
    0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00,  // ................
    0x02, 0x00, 0x00, 0x00, 0xcc, 0x01, 0x00, 0x00, 0x04, 0x00, 0x0d, 0x00, 0x47, 0x4c, 0x5f, 0x45,  // ............GL_E
    0x58, 0x54, 0x5f, 0x73, 0x68, 0x61, 0x64, 0x65, 0x72, 0x5f, 0x65, 0x78, 0x70, 0x6c, 0x69, 0x63,  // XT_shader_explic
    0x69, 0x74, 0x5f, 0x61, 0x72, 0x69, 0x74, 0x68, 0x6d, 0x65, 0x74, 0x69, 0x63, 0x5f, 0x74, 0x79,  // it_arithmetic_ty
    0x70, 0x65, 0x73, 0x5f, 0x69, 0x6e, 0x74, 0x33, 0x32, 0x00, 0x00, 0x00, 0x04, 0x00, 0x0d, 0x00,  // pes_int32.......
    0x47, 0x4c, 0x5f, 0x45, 0x58, 0x54, 0x5f, 0x73, 0x68, 0x61, 0x64, 0x65, 0x72, 0x5f, 0x65, 0x78,  // GL_EXT_shader_ex
    0x70, 0x6c, 0x69, 0x63, 0x69, 0x74, 0x5f, 0x61, 0x72, 0x69, 0x74, 0x68, 0x6d, 0x65, 0x74, 0x69,  // plicit_arithmeti
    0x63, 0x5f, 0x74, 0x79, 0x70, 0x65, 0x73, 0x5f, 0x69, 0x6e, 0x74, 0x38, 0x00, 0x00, 0x00, 0x00,  // c_types_int8....
    0x05, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00,  // ........main....
    0x05, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x4f, 0x75, 0x74, 0x70, 0x75, 0x74, 0x00, 0x00,  // ........Output..
    0x06, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00,  // ............x...
    0x05, 0x00, 0x03, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x6f, 0x00, 0x00, 0x00, 0x05, 0x00, 0x08, 0x00,  // ........o.......
    0x10, 0x00, 0x00, 0x00, 0x67, 0x6c, 0x5f, 0x47, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x49, 0x6e, 0x76,  // ....gl_GlobalInv
    0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x49, 0x44, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00,  // ocationID.......
    0x16, 0x00, 0x00, 0x00, 0x49, 0x6e, 0x70, 0x75, 0x74, 0x41, 0x00, 0x00, 0x06, 0x00, 0x04, 0x00,  // ....InputA......
    0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00,  // ........x.......
    0x18, 0x00, 0x00, 0x00, 0x69, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00,  // ....i...G.......
    0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x08, 0x00, 0x00, 0x00,  // ........H.......
    0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x03, 0x00,  // ....#.......G...
    0x08, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00,  // ........G.......
    0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00,  // ".......G.......
    0x21, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x10, 0x00, 0x00, 0x00,  // !.......G.......
    0x0b, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x15, 0x00, 0x00, 0x00,  // ........G.......
    0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x16, 0x00, 0x00, 0x00,  // ........H.......
    0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x03, 0x00,  // ....#.......G...
    0x16, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x18, 0x00, 0x00, 0x00,  // ........G.......
    0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x18, 0x00, 0x00, 0x00,  // ".......G.......
    0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00,  // !...............
    0x21, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x15, 0x00, 0x04, 0x00,  // !...............
    0x06, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x03, 0x00,  // ................
    0x07, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x03, 0x00, 0x08, 0x00, 0x00, 0x00,  // ................
    0x07, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,  // .... ...........
    0x08, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,  // ....;...........
    0x02, 0x00, 0x00, 0x00, 0x15, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,  // ............ ...
    0x01, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,  // ....+...........
    0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x04, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,  // ............ ...
    0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00,  // ................
    0x03, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,  // .... ...........
    0x0e, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,  // ....;...........
    0x01, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00,  // ....+...........
    0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x12, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,  // .... ...........
    0x0d, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x03, 0x00, 0x15, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,  // ................
    0x1e, 0x00, 0x03, 0x00, 0x16, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,  // ............ ...
    0x17, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00,  // ............;...
    0x17, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,  // ............ ...
    0x1b, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x36, 0x00, 0x05, 0x00,  // ............6...
    0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,  // ................
    0xf8, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x12, 0x00, 0x00, 0x00,  // ........A.......
    0x13, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00,  // ............=...
    0x0d, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00,  // ............A...
    0x12, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00,  // ................
    0x3d, 0x00, 0x04, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00,  // =...............
    0x41, 0x00, 0x06, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,  // A...............
    0x0c, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00,  // ........=.......
    0x1d, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x12, 0x00, 0x00, 0x00,  // ........A.......
    0x1e, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00,  // ............=...
    0x0d, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x41, 0x00, 0x06, 0x00,  // ............A...
    0x1b, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,  // .... ...........
    0x1f, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00,  // ....=.......!...
    0x20, 0x00, 0x00, 0x00, 0x84, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00,  //  ..........."...
    0x1d, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x41, 0x00, 0x06, 0x00, 0x1b, 0x00, 0x00, 0x00,  // ....!...A.......
    0x23, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00,  // #...............
    0x3e, 0x00, 0x03, 0x00, 0x23, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x01, 0x00,  // >...#...".......
    0x38, 0x00, 0x01, 0x00 // 8...
};

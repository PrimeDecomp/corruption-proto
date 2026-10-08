/*
 * G2MEAB Collision/CDelaunayTriangulation.cpp translation-unit scaffold.
 * .text: 0x8047F4A0..0x804801DC (15 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: All15natives comprise one coherent Delaunay triangulation family. Leading7F4A0
 * processes open oriented edges, finds a third point via7F6B4 and appends triangle-index triples
 * (stride0xC) with vector.h482 assertion;7F6B4 tests side-of-edge and empty circumcircle,7F87C
 * computes circumcenter/radius from three double2D points;7F9F0 finds nearest initial point
 * pair;7FAA0 links adjacent triangles;7FAEC finds edge;7FB58/7FBF0 append edges
 * (stride0x10,asserted vector push);7FCE4 drives triangulation;7FDB0 builds three vectors from
 * input points, converting first two float coordinates to doubles. Preserve triangle
 * reserve/copy7FF34/7FFEC, double2D reserve80030 and edge reserve/copy800D8/80190+4C. Prior7F438
 * closes CharacterPrimitiveData box copying; next801DC starts a distinct primitive vtable getter.
 * Neither Prime nor Echoes has a named triangulation source/header/native TU; ordinary
 * vector-helper broad similarities are generic and not transplanted identities.
 * CDelaunayTriangulation is an inferred descriptive original-TU filename, not recovered debug
 * provenance.
 */

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted

extern "C" void fn_8047FAA0(int obj, int val, int val2, int val3, int val4);

extern "C" void fn_8047FAA0(int obj, int val, int val2, int val3, int val4) {
    int val5 = *(int*)((char*)obj + 0x2c) + (val << 4);
    if (val2 == (*(int*)val5) && (*(int*)((char*)val5 + 0x8)) == -1) {
        *(int*)((char*)val5 + 0x8) = val4;
    } else {
        if (val2 != (*(int*)((char*)val5 + 0x4))) {
            return;
        }
        if ((*(int*)((char*)val5 + 0xc)) != -1) {
            return;
        }
        *(int*)((char*)val5 + 0xc) = val4;
    }
}


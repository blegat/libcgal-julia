#include <jlcxx/module.hpp>

#include <julia.h>

#include "macros.hpp"
#include "triangulation.hpp"
#include "utils.hpp"

#define WRAP_TRIANGULATION(T, JT) \
  /* Creation */ \
  CTOR(const T&) \
  .METHOD(T, swap) \
  /* Access Functions */ \
  .METHOD(T, dimension) \
  .METHOD(T, number_of_faces) \
  .METHOD(T, number_of_vertices) \
  /* Queries */ \
  .method("locate", [](const T& t, const T::Point& query) { \
    T::Face_handle fh = t.locate(query); \
    return fh != nullptr ? \
      (jl_value_t*)jlcxx::box<T::Face>(*fh) : \
      jl_nothing; \
  }) \
  .method("inexact_locate", [](const T& t, const T::Point& query) { \
    T::Face_handle fh = t.inexact_locate(query); \
    return fh != nullptr ? \
      (jl_value_t*)jlcxx::box<T::Face>(*fh) : \
      jl_nothing; \
  }) \
  /* Modifiers */ \
  OVERRIDE_BASE(cgal, JT) \
  .method("empty!", [](T& t) { \
    t.clear(); \
    return t; \
  }) \
  .method("insert!", [](T& t, jlcxx::ArrayRef<T::Point> ps) { \
    t.insert(ps.begin(), ps.end()); \
    return t; \
  }) \
  .method("push!", [](T& t, const T::Point& p) { \
    t.push_back(p); \
    return t; \
  }) \
  UNSET_OVERRIDE(cgal, JT) \
  /* Finite Face, Edge and Vertex Iterators */ \
  .method("vertices", [](const T& t) { \
    return collect(t.vertices_begin(), t.vertices_end()); \
  }) \
  .method("edges", [](const T& t) { \
    return collect(t.edges_begin(), t.edges_end()); \
  }) \
  .method("faces", [](const T& t) { \
    return collect(t.faces_begin(), t.faces_end()); \
  }) \
  .method("points", [](const T& t) { \
    return collect(t.points_begin(), t.points_end()); \
  }) \
  /* All Face, Edge and Vertex Iterators */ \
  .method("all_vertices", [](const T& t) { \
    return collect(t.all_vertices_begin(), t.all_vertices_end()); \
  }) \
  .method("all_edges", [](const T& t) { \
    return collect(t.all_edges_begin(), t.all_edges_end()); \
  }) \
  .method("all_faces", [](const T& t) { \
    return collect(t.all_faces_begin(), t.all_faces_end()); \
  }) \
  /* Line Face Circulator */ \
  .method("line_walk", [](const T& t, const T::Point& p, const T::Point& q) { \
    return collect(t.line_walk(p, q)); \
  }) \
  /* Traversal Between Adjacent Faces */ \
  .METHOD(T, mirror_edge) \
  /* Miscellaneous */ \
  .UNAMBIG_METHOD(T::Segment, T, segment, const T::Edge&) \
  /* Checking */ \
  .METHOD(T, is_valid)

namespace jlcxx {
  using namespace jlcgal;

  template<> struct SuperType<CTr_2::Edge>   { typedef CTr_2::Triangulation::Edge type; };
  template<> struct SuperType<CTr_2::Face>   { typedef CTr_2::Triangulation::Face type; };
  template<> struct SuperType<CTr_2::Vertex> { typedef CTr_2::Triangulation::Vertex type; };

  template<> struct SuperType<CDTr_2> { typedef CTr_2 type; };

  template<> struct SuperType<DTr_2>  { typedef Tr_2 type; };

  template<> struct SuperType<RTr_2::Edge>   { typedef RTr_2::Triangulation_base::Edge type; };
  template<> struct SuperType<RTr_2::Face>   { typedef RTr_2::Triangulation_base::Face type; };
  template<> struct SuperType<RTr_2::Vertex> { typedef RTr_2::Triangulation_base::Vertex type; };
}

namespace jlcgal {

void wrap_triangulation_2(jlcxx::Module& cgal) {
  const std::string tr_name = "Triangulation2";
  auto tr      = cgal.add_type<Tr_2>        (tr_name);
  auto tedge   = cgal.add_type<Tr_2::Edge>  (tr_name + "Edge");
  auto tface   = cgal.add_type<Tr_2::Face>  (tr_name + "Face");
  auto tvertex = cgal.add_type<Tr_2::Vertex>(tr_name + "Vertex");

  const std::string ctr_name = "Constrained" + tr_name;
  cgal.add_type<CTr_2::Triangulation>(ctr_name + "Base");
  auto ctr      = cgal.add_type<CTr_2>        (ctr_name);
  auto ctedge   = cgal.add_type<CTr_2::Edge>  (ctr_name + "Edge",   tedge.dt());
  auto ctface   = cgal.add_type<CTr_2::Face>  (ctr_name + "Face",   tface.dt());
  auto ctvertex = cgal.add_type<CTr_2::Vertex>(ctr_name + "Vertex", tvertex.dt());

  auto cdtr = cgal.add_type<CDTr_2>("ConstrainedDelaunay" + tr_name, ctr.dt());

  const std::string dtr_name = "Delaunay" + tr_name;
  auto dtr = cgal.add_type<DTr_2>(dtr_name, tr.dt());

  const std::string rtr_name = "Regular" + tr_name;
  cgal.add_type<RTr_2::Triangulation_base>(rtr_name + "Base");
  auto rtr      = cgal.add_type<RTr_2>        (rtr_name);
  auto rtedge   = cgal.add_type<RTr_2::Edge>  (rtr_name + "Edge",   tedge.dt());
  auto rtface   = cgal.add_type<RTr_2::Face>  (rtr_name + "Face",   tface.dt());
  auto rtvertex = cgal.add_type<RTr_2::Vertex>(rtr_name + "Vertex", tvertex.dt());

  tvertex
    .METHOD(Tr_2::Vertex, degree)
    .UNAMBIG_METHOD(const Tr_2::Point&, Tr_2::Vertex, point)
    ;

  tface
    .METHOD(Tr_2::Face, dimension)
    .METHOD(Tr_2::Face, is_valid)
    .method("neighbor", [](const Tr_2::Face& f, const jlcxx::cxxint_t i) {
      return *f.neighbor(i - 1);
    })
    .method("vertex", [](const Tr_2::Face& f, const jlcxx::cxxint_t i) {
      return *f.vertex(i - 1);
    })
    ;

  tr
    .WRAP_TRIANGULATION(Tr_2, tr)
    .method(tr_name, [](jlcxx::ArrayRef<Tr_2::Point> ps) {
      return jlcxx::create<Tr_2>(ps.begin(), ps.end());
    })
    ;

  ctr
    .WRAP_TRIANGULATION(CTr_2, ctr)
    // Queries
    .METHOD(CTr_2, is_constrained)
    .method("constrained_edges", [](const CTr_2& ct) {
      return collect(ct.constrained_edges_begin(), ct.constrained_edges_end());
    })
    .method("insert_constraint", CAST_METHOD(void, CTr_2, insert_constraint,,
                                                        const CTr_2::Point&,
                                                        const CTr_2::Point&))
    .method("insert_constraint", [](CTr_2& ct, jlcxx::ArrayRef<CTr_2::Point> ps) {
      ct.insert_constraint(ps.begin(), ps.end());
    })
    ;

  cdtr
    // Creation
    .CTOR(const CDTr_2&)
    // Insertion and Removal
    OVERRIDE_BASE(cgal, cdtr)
    .method("insert!", [](CDTr_2& cdtr, jlcxx::ArrayRef<CDTr_2::Point> ps) {
      cdtr.insert(ps.begin(), ps.end());
      return cdtr;
    })
    .method("push!", [](CDTr_2& cdtr, const CDTr_2::Point& p) {
      cdtr.push_back(p);
      return cdtr;
    })
    UNSET_OVERRIDE(cgal, cdtr)
    // Miscellaneous
    .METHOD(CDTr_2, is_valid)
    ;

  dtr
    // Creation
    .CTOR(const DTr_2&)
    .method(dtr_name, [](jlcxx::ArrayRef<DTr_2::Point> ps) {
      return jlcxx::create<DTr_2>(ps.begin(), ps.end());
    })
    // Insertion and Removal
    OVERRIDE_BASE(cgal, dtr)
    .method("insert!", [](DTr_2& dt, jlcxx::ArrayRef<DTr_2::Point> ps) {
      dt.insert(ps.begin(), ps.end());
      return dt;
    })
    .method("push!", [](DTr_2& dt, const DTr_2::Point& p) {
      dt.push_back(p);
      return dt;
    })
    UNSET_OVERRIDE(cgal, dtr)
    // Queries
    .method("nearest_vertex", [](const DTr_2& dt, const DTr_2::Point& p) {
      return *dt.nearest_vertex(p);
    })
    // Voronoi Diagram
    .method("dual", [](const DTr_2& dt, const Tr_2::Edge& e) {
      auto&& o = dt.dual(e);

      if (const Line_2* l = CGAL::object_cast<Line_2>(&o)) {
        return (jl_value_t*)jlcxx::box<Line_2>(*l);
      } else if (const Ray_2* r = CGAL::object_cast<Ray_2>(&o)) {
        return (jl_value_t*)jlcxx::box<Ray_2>(*r);
      } else if (const Segment_2* s = CGAL::object_cast<Segment_2>(&o)) {
        return (jl_value_t*)jlcxx::box<Segment_2>(*s);
      }

      return jl_nothing; // unreachable
    })
    // Miscellaneous
    .METHOD(DTr_2, is_valid)
    ;

  rtvertex
    .UNAMBIG_METHOD(const RTr_2::Point&, RTr_2::Vertex, point)
    .METHOD(RTr_2::Vertex, is_hidden)
    .METHOD(RTr_2::Vertex, set_hidden)
    ;

  rtr
    .WRAP_TRIANGULATION(RTr_2, rtr)
    .method(rtr_name, [](jlcxx::ArrayRef<RTr_2::Point> ps) {
      return jlcxx::create<RTr_2>(ps.begin(), ps.end());
    })
    // Queries
    .method("nearest_power_vertex", [](const RTr_2& rt, const RTr_2::Bare_point& p) {
      return *rt.nearest_power_vertex(p);
    })
    // Access Functions
    .METHOD(RTr_2, number_of_hidden_vertices)
    .method("hidden_vertices", [](const RTr_2& rt) {
      return collect(rt.hidden_vertices_begin(), rt.hidden_vertices_end());
    })
    .method("finite_vertices", [](const RTr_2& rt) {
      return collect(rt.finite_vertices_begin(), rt.finite_vertices_end());
    })
    // Dual Power Diagram
    .method("dual", [](const RTr_2& rt, const RTr_2::Edge& e) {
      auto&& o = rt.dual(e);

      if (const Line_2* l = CGAL::object_cast<Line_2>(&o)) {
        return (jl_value_t*)jlcxx::box<Line_2>(*l);
      } else if (const Ray_2* r = CGAL::object_cast<Ray_2>(&o)) {
        return (jl_value_t*)jlcxx::box<Ray_2>(*r);
      } else if (const Segment_2* s = CGAL::object_cast<Segment_2>(&o)) {
        return (jl_value_t*)jlcxx::box<Segment_2>(*s);
      }

      return jl_nothing; // unreachable
    })
    ;
}

#undef WRAP_TRIANGULATION

} // jlcgal

#include <pybind11/pybind11.h>
#include <glm/glm.hpp>

namespace PYBIND11_NAMESPACE { namespace detail {
        template <> struct type_caster<glm::vec2> {
        public:
            /**
             * This macro establishes the name 'inty' in
             * function signatures and declares a local variable
             * 'value' of type inty
             */
        PYBIND11_TYPE_CASTER(glm::vec2, const_name("vec2"));

            /**
             * Conversion part 1 (Python->C++): convert a PyObject into a inty
             * instance or return false upon failure. The second argument
             * indicates whether implicit conversions should be applied.
             */
            bool load(handle src, bool) {
                /* Extract PyObject from handle */
                //PyObject *source = src.ptr();
                auto tuple = src.cast<pybind11::tuple>();
                value.x = tuple[0].cast<float>();
                value.y = tuple[1].cast<float>();

                /* Ensure return code was OK (to avoid out-of-range errors etc) */
                return true;
            }

            /**
             * Conversion part 2 (C++ -> Python): convert an inty instance into
             * a Python object. The second and third arguments are used to
             * indicate the return value policy and parent object (for
             * ``return_value_policy::reference_internal``) and are generally
             * ignored by implicit casters.
             */
            static handle cast(glm::vec2 src, return_value_policy /* policy */, handle /* parent */) {
                return PyTuple_Pack(2, src.x, src.y);// PyLong_FromLong(src.long_value);
            }
        };
		template <> struct type_caster<glm::vec3> {
		public:
			/**
			 * This macro establishes the name 'inty' in
			 * function signatures and declares a local variable
			 * 'value' of type inty
			 */
			PYBIND11_TYPE_CASTER(glm::vec3, const_name("vec3"));

			/**
			 * Conversion part 1 (Python->C++): convert a PyObject into a inty
			 * instance or return false upon failure. The second argument
			 * indicates whether implicit conversions should be applied.
			 */
			bool load(handle src, bool) {
				/* Extract PyObject from handle */
				//PyObject *source = src.ptr();
				auto tuple = src.cast<pybind11::tuple>();
				value.x = tuple[0].cast<float>();
				value.y = tuple[1].cast<float>();
				value.z = tuple[2].cast<float>();
				/* Ensure return code was OK (to avoid out-of-range errors etc) */
				return true;
			}

			/**
			 * Conversion part 2 (C++ -> Python): convert an inty instance into
			 * a Python object. The second and third arguments are used to
			 * indicate the return value policy and parent object (for
			 * ``return_value_policy::reference_internal``) and are generally
			 * ignored by implicit casters.
			 */
			static handle cast(glm::vec3 src, return_value_policy /* policy */, handle /* parent */) {
				return PyTuple_Pack(3, src.x, src.y, src.z);// PyLong_FromLong(src.long_value);
			}
		};
        template <> struct type_caster<glm::vec4> {
        public:
            /**
             * This macro establishes the name 'inty' in
             * function signatures and declares a local variable
             * 'value' of type inty
             */
        PYBIND11_TYPE_CASTER(glm::vec4, const_name("vec4"));

            /**
             * Conversion part 1 (Python->C++): convert a PyObject into a inty
             * instance or return false upon failure. The second argument
             * indicates whether implicit conversions should be applied.
             */
            bool load(handle src, bool) {
                /* Extract PyObject from handle */
                //PyObject *source = src.ptr();
                auto tuple = src.cast<pybind11::tuple>();
                value.x = tuple[0].cast<float>();
                value.y = tuple[1].cast<float>();
                value.z = tuple[2].cast<float>();
                value.w = tuple[3].cast<float>();
                /* Ensure return code was OK (to avoid out-of-range errors etc) */
                return true;
            }

            /**
             * Conversion part 2 (C++ -> Python): convert an inty instance into
             * a Python object. The second and third arguments are used to
             * indicate the return value policy and parent object (for
             * ``return_value_policy::reference_internal``) and are generally
             * ignored by implicit casters.
             */
            static handle cast(glm::vec4 src, return_value_policy /* policy */, handle /* parent */) {
                return PyTuple_Pack(3, src.x, src.y, src.z, src.w);// PyLong_FromLong(src.long_value);
            }
        };
		template <> struct type_caster<glm::ivec2> {
		public:
			/**
			 * This macro establishes the name 'inty' in
			 * function signatures and declares a local variable
			 * 'value' of type inty
			 */
		PYBIND11_TYPE_CASTER(glm::ivec2, const_name("ivec2"));

			/**
			 * Conversion part 1 (Python->C++): convert a PyObject into a inty
			 * instance or return false upon failure. The second argument
			 * indicates whether implicit conversions should be applied.
			 */
			bool load(handle src, bool) {
				/* Extract PyObject from handle */
				//PyObject *source = src.ptr();
				auto tuple = src.cast<pybind11::tuple>();
				value.x = tuple[0].cast<int>();
				value.y = tuple[1].cast<int>();

				/* Ensure return code was OK (to avoid out-of-range errors etc) */
				return true;
			}

			/**
			 * Conversion part 2 (C++ -> Python): convert an inty instance into
			 * a Python object. The second and third arguments are used to
			 * indicate the return value policy and parent object (for
			 * ``return_value_policy::reference_internal``) and are generally
			 * ignored by implicit casters.
			 */
			static handle cast(glm::ivec2 src, return_value_policy /* policy */, handle /* parent */) {
				return PyTuple_Pack(2, src.x, src.y);// PyLong_FromLong(src.long_value);
			}
		};
		template <> struct type_caster<glm::ivec3> {
		public:
			/**
			 * This macro establishes the name 'inty' in
			 * function signatures and declares a local variable
			 * 'value' of type inty
			 */
		PYBIND11_TYPE_CASTER(glm::ivec3, const_name("ivec3"));

			/**
			 * Conversion part 1 (Python->C++): convert a PyObject into a inty
			 * instance or return false upon failure. The second argument
			 * indicates whether implicit conversions should be applied.
			 */
			bool load(handle src, bool) {
				/* Extract PyObject from handle */
				//PyObject *source = src.ptr();
				auto tuple = src.cast<pybind11::tuple>();
				value.x = tuple[0].cast<int>();
				value.y = tuple[1].cast<int>();
				value.z = tuple[2].cast<int>();
				/* Ensure return code was OK (to avoid out-of-range errors etc) */
				return true;
			}

			/**
			 * Conversion part 2 (C++ -> Python): convert an inty instance into
			 * a Python object. The second and third arguments are used to
			 * indicate the return value policy and parent object (for
			 * ``return_value_policy::reference_internal``) and are generally
			 * ignored by implicit casters.
			 */
			static handle cast(glm::ivec3 src, return_value_policy /* policy */, handle /* parent */) {
				return PyTuple_Pack(3, src.x, src.y, src.z);// PyLong_FromLong(src.long_value);
			}
		};
		template <> struct type_caster<glm::ivec4> {
		public:
			/**
			 * This macro establishes the name 'inty' in
			 * function signatures and declares a local variable
			 * 'value' of type inty
			 */
		PYBIND11_TYPE_CASTER(glm::ivec4, const_name("ivec4"));

			/**
			 * Conversion part 1 (Python->C++): convert a PyObject into a inty
			 * instance or return false upon failure. The second argument
			 * indicates whether implicit conversions should be applied.
			 */
			bool load(handle src, bool) {
				/* Extract PyObject from handle */
				//PyObject *source = src.ptr();
				auto tuple = src.cast<pybind11::tuple>();
				value.x = tuple[0].cast<int>();
				value.y = tuple[1].cast<int>();
				value.z = tuple[2].cast<int>();
				value.w = tuple[3].cast<int>();
				/* Ensure return code was OK (to avoid out-of-range errors etc) */
				return true;
			}

			/**
			 * Conversion part 2 (C++ -> Python): convert an inty instance into
			 * a Python object. The second and third arguments are used to
			 * indicate the return value policy and parent object (for
			 * ``return_value_policy::reference_internal``) and are generally
			 * ignored by implicit casters.
			 */
			static handle cast(glm::ivec4 src, return_value_policy /* policy */, handle /* parent */) {
				return PyTuple_Pack(4, src.x, src.y, src.z, src.w);// PyLong_FromLong(src.long_value);
			}
		};
	}} // namespace PYBIND11_NAMESPACE::detail
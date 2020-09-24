/* -*- c++ -*- */

#define SANDIA_UTILS_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "sandia_utils_swig_doc.i"

%{
#include "sandia_utils/block_buffer.h"
#include "sandia_utils/burst_power_detector.h"
#include "sandia_utils/interleaved_short_to_complex.h"
#include "sandia_utils/complex_to_interleaved_short.h"
#include "sandia_utils/file_sink.h"
#include "sandia_utils/constants.h"
#include "sandia_utils/invert_tune.h"
#include "sandia_utils/file_source.h"
#include "sandia_utils/message_vector_file_sink.h"
#include "sandia_utils/message_vector_raster_file_sink.h"
#include "sandia_utils/stream_gate.h"
#include "sandia_utils/tag_debug_file.h"
#include "sandia_utils/sandia_tag_debug.h"
#include "sandia_utils/tagged_bits_to_bytes.h"
#include "sandia_utils/compute_stats.h"
#include "sandia_utils/vita49_tcp_msg_source.h"
%}

%include "sandia_utils/constants.h"


%include "sandia_utils/block_buffer.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, block_buffer);
%include "sandia_utils/burst_power_detector.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, burst_power_detector);
%include "sandia_utils/interleaved_short_to_complex.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, interleaved_short_to_complex);
%include "sandia_utils/complex_to_interleaved_short.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, complex_to_interleaved_short);
%include "sandia_utils/file_sink.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, file_sink);
%include "sandia_utils/invert_tune.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, invert_tune);
%include "sandia_utils/file_source.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, file_source);
%include "sandia_utils/message_vector_file_sink.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, message_vector_file_sink);
%include "sandia_utils/message_vector_raster_file_sink.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, message_vector_raster_file_sink);
%include "sandia_utils/tag_debug_file.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, tag_debug_file);
%include "sandia_utils/sandia_tag_debug.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, sandia_tag_debug);
%include "sandia_utils/tagged_bits_to_bytes.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, tagged_bits_to_bytes);

// stream gate
%include "sandia_utils/stream_gate.h"
GR_SWIG_BLOCK_MAGIC2_TMPL(sandia_utils, stream_gate_b, stream_gate<unsigned char>);
GR_SWIG_BLOCK_MAGIC2_TMPL(sandia_utils, stream_gate_s, stream_gate<short>);
GR_SWIG_BLOCK_MAGIC2_TMPL(sandia_utils, stream_gate_i, stream_gate<int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(sandia_utils, stream_gate_f, stream_gate<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(sandia_utils, stream_gate_c, stream_gate<gr_complex>);
%include "sandia_utils/compute_stats.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, compute_stats);
%include "sandia_utils/vita49_tcp_msg_source.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, vita49_tcp_msg_source);

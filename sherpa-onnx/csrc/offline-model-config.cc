// sherpa-onnx/csrc/offline-model-config.cc
//
// Copyright (c)  2023  Xiaomi Corporation
#include "sherpa-onnx/csrc/offline-model-config.h"

#include <string>

#include "sherpa-onnx/csrc/file-utils.h"
#include "sherpa-onnx/csrc/macros.h"

namespace sherpa_onnx {

void OfflineModelConfig::Register(ParseOptions *po) {
  transducer.Register(po);
  paraformer.Register(po);
  nemo_ctc.Register(po);
  whisper.Register(po);
  fire_red_asr.Register(po);
  tdnn.Register(po);
  zipformer_ctc.Register(po);
  wenet_ctc.Register(po);
  sense_voice.Register(po);
  moonshine.Register(po);

  po->Register("telespeech-ctc", &telespeech_ctc,
               "Path to model.onnx for telespeech ctc");

  po->Register("tokens", &tokens, "Path to tokens.txt");

  po->Register("num-threads", &num_threads,
               "Number of threads to run the neural network");

  po->Register("debug", &debug,
               "true to print model information while loading it.");

  po->Register("provider", &provider,
               "Specify a provider to use: cpu, cuda, coreml");

  po->Register("model-type", &model_type,
               "Specify it to reduce model initialization time. "
               "Valid values are: transducer, paraformer, nemo_ctc, whisper, "
               "tdnn, zipformer2_ctc, telespeech_ctc, fire_red_asr."
               "All other values lead to loading the model twice.");
  po->Register("modeling-unit", &modeling_unit,
               "The modeling unit of the model, commonly used units are bpe, "
               "cjkchar, cjkchar+bpe, etc. Currently, it is needed only when "
               "hotwords are provided, we need it to encode the hotwords into "
               "token sequence.");
  po->Register("bpe-vocab", &bpe_vocab,
               "The vocabulary generated by google's sentencepiece program. "
               "It is a file has two columns, one is the token, the other is "
               "the log probability, you can get it from the directory where "
               "your bpe model is generated. Only used when hotwords provided "
               "and the modeling unit is bpe or cjkchar+bpe");
}

bool OfflineModelConfig::Validate() const {
  if (num_threads < 1) {
    SHERPA_ONNX_LOGE("num_threads should be > 0. Given %d", num_threads);
    return false;
  }

  if (!FileExists(tokens)) {
    SHERPA_ONNX_LOGE("tokens: '%s' does not exist", tokens.c_str());
    return false;
  }

  if (!modeling_unit.empty() &&
      (modeling_unit == "bpe" || modeling_unit == "cjkchar+bpe")) {
    if (!FileExists(bpe_vocab)) {
      SHERPA_ONNX_LOGE("bpe_vocab: '%s' does not exist", bpe_vocab.c_str());
      return false;
    }
  }

  if (!paraformer.model.empty()) {
    return paraformer.Validate();
  }

  if (!nemo_ctc.model.empty()) {
    return nemo_ctc.Validate();
  }

  if (!whisper.encoder.empty()) {
    return whisper.Validate();
  }

  if (!fire_red_asr.encoder.empty()) {
    return fire_red_asr.Validate();
  }

  if (!tdnn.model.empty()) {
    return tdnn.Validate();
  }

  if (!zipformer_ctc.model.empty()) {
    return zipformer_ctc.Validate();
  }

  if (!wenet_ctc.model.empty()) {
    return wenet_ctc.Validate();
  }

  if (!sense_voice.model.empty()) {
    return sense_voice.Validate();
  }

  if (!moonshine.preprocessor.empty()) {
    return moonshine.Validate();
  }

  if (!telespeech_ctc.empty() && !FileExists(telespeech_ctc)) {
    SHERPA_ONNX_LOGE("telespeech_ctc: '%s' does not exist",
                     telespeech_ctc.c_str());
    return false;
  }

  if (!transducer.encoder_filename.empty()) {
    return transducer.Validate();
  }

  return true;
}

std::string OfflineModelConfig::ToString() const {
  std::ostringstream os;

  os << "OfflineModelConfig(";
  os << "transducer=" << transducer.ToString() << ", ";
  os << "paraformer=" << paraformer.ToString() << ", ";
  os << "nemo_ctc=" << nemo_ctc.ToString() << ", ";
  os << "whisper=" << whisper.ToString() << ", ";
  os << "fire_red_asr=" << fire_red_asr.ToString() << ", ";
  os << "tdnn=" << tdnn.ToString() << ", ";
  os << "zipformer_ctc=" << zipformer_ctc.ToString() << ", ";
  os << "wenet_ctc=" << wenet_ctc.ToString() << ", ";
  os << "sense_voice=" << sense_voice.ToString() << ", ";
  os << "moonshine=" << moonshine.ToString() << ", ";
  os << "telespeech_ctc=\"" << telespeech_ctc << "\", ";
  os << "tokens=\"" << tokens << "\", ";
  os << "num_threads=" << num_threads << ", ";
  os << "debug=" << (debug ? "True" : "False") << ", ";
  os << "provider=\"" << provider << "\", ";
  os << "model_type=\"" << model_type << "\", ";
  os << "modeling_unit=\"" << modeling_unit << "\", ";
  os << "bpe_vocab=\"" << bpe_vocab << "\")";

  return os.str();
}

}  // namespace sherpa_onnx

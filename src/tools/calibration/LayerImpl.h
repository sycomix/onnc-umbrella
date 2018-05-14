//===- LayerImpl.h --------------------------------------------------------===//
//
//                             The ONNC Project
//
// See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
void Calibration::Conv(const OperatorDef &pOp, caffe2::NetDef &pDef)
{
  LayerCalibrationParameter *layerCalibrationParam =
      m_NetCtableParam.add_layer();

  const string &inputName = pOp.input(0);
  const string &weightName = pOp.input(1);
  const string &outputName = pOp.output(0);

  float thresX = m_ThresholdY[inputName];
  float thresY = m_ThresholdY[outputName];

  // Get weight and calculate right shift.
  auto wBlob = m_Workspace->GetBlob(weightName);
  int rightShift = getRightShift(wBlob, thresX / thresY);

  // Quantize the weights.
  quantizeWeight(wBlob, thresX, thresY, rightShift, weightName);

  if (pOp.input_size() == 3) {
    const string &biasName = pOp.input(2);
    auto bBlob = m_Workspace->GetBlob(biasName);
    quantizeBias(bBlob, 128, thresY, rightShift, weightName);
  }

  // Setup Ctable parameters.
  BlobParameter *outBlobParam = layerCalibrationParam->add_blob_param();
  outBlobParam->set_name(outputName);
  outBlobParam->set_threshold_y(thresY);
  layerCalibrationParam->set_right_shift_width(rightShift);
  layerCalibrationParam->add_threshold_y(thresY);
}

#include "TGRelu.h"
#include "BM168xCodeEmitter.h"
#include <bmkernel_api.h>

using namespace onnc;

TGRelu::TGRelu(const ::onnx::Node &pNode, MemTable &pMemTable)
    : Operator(pNode, "Relu"), m_negativeSlope(0)
{
  const std::vector< ::onnx::Dimension> inDim = pNode.inputs()[0]->sizes();

  auto inputs = pNode.inputs();
  auto outputs = pNode.outputs();
  m_inputAddr = pMemTable[inputs[0]->uniqueName()];
  m_outputAddr = pMemTable[outputs[0]->uniqueName()];

  if (inDim.size() == 4) {
    m_N = inDim[0].dim;
    m_C = inDim[1].dim;
    m_H = inDim[2].dim;
    m_W = inDim[3].dim;
  } else if (inDim.size() == 2) {
    m_N = 1;
    m_C = 1;
    m_H = inDim[0].dim;
    m_W = inDim[1].dim;
  } else {
    assert(0 && "inDim.size() != 4 & !=2");
  }
}

void TGRelu::emit() const
{
  std::cout << "TGRelu::emit\tm_inputAddr:" << m_inputAddr
            << " m_outputAddr:" << m_outputAddr
            << " m_negativeSlope:" << m_negativeSlope << " m_N:" << m_N
            << " m_C:" << m_C << " m_H:" << m_H << " m_W:" << m_W << std::endl;
  bmnet::bmnet_relu_forward_bmkernel(*bm168x_kernel::getInstance().ctx,
                                     m_inputAddr, m_outputAddr, m_negativeSlope,
                                     m_N, m_C, m_H, m_W);
}

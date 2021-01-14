#pragma once

#include <JuceHeader.h>

#ifndef DOXYGEN
/** The contents of this namespace are used to implement ProcessorChain and should
    not be used elsewhere. Their interfaces (and existence) are liable to change!
*/
namespace procchain_detail
{
template <typename Fn, typename Tuple, size_t... Ix>
constexpr void forEachInTuple (Fn&& fn, Tuple&& tuple, std::index_sequence<Ix...>) noexcept (noexcept (std::initializer_list<int> { (fn (std::get<Ix> (tuple), Ix), 0)... }))
{
    (void) std::initializer_list<int> { ((void) fn (std::get<Ix> (tuple), Ix), 0)... };
}

template <typename T>
using TupleIndexSequence = std::make_index_sequence<std::tuple_size<std::remove_cv_t<std::remove_reference_t<T>>>::value>;

template <typename Fn, typename Tuple>
constexpr void forEachInTuple (Fn&& fn, Tuple&& tuple) noexcept (noexcept (forEachInTuple (std::forward<Fn> (fn), std::forward<Tuple> (tuple), TupleIndexSequence<Tuple> {})))
{
    forEachInTuple (std::forward<Fn> (fn), std::forward<Tuple> (tuple), TupleIndexSequence<Tuple> {});
}
} // namespace procchain_detail
#endif

/** This variadically-templated class lets you join together any number of processor
    classes into a single processor which will call process() on them all in sequence.

    @tags{DSP}
*/
template <typename... Processors>
class MyProcessorChain
{
public:
    /** Get a reference to the processor at index `Index`. */
    template <int Index>
    auto& get() noexcept
    {
        return std::get<Index> (processors);
    }

    /** Get a reference to the processor at index `Index`. */
    template <int Index>
    const auto& get() const noexcept
    {
        return std::get<Index> (processors);
    }

    /** Set the processor at index `Index` to be bypassed or enabled. */
    template <int Index>
    void setBypassed (bool b) noexcept
    {
        bypassed[(size_t) Index] = b;
    }

    /** Query whether the processor at index `Index` is bypassed. */
    template <int Index>
    bool isBypassed() const noexcept
    {
        return bypassed[(size_t) Index];
    }

    /** Prepare all inner processors with the provided `ProcessSpec`. */
    void prepare (const dsp::ProcessSpec& spec)
    {
        procchain_detail::forEachInTuple ([&] (auto& proc, size_t) { proc.prepare (spec); }, processors);
    }

    /** Reset all inner processors. */
    void reset()
    {
        procchain_detail::forEachInTuple ([] (auto& proc, size_t) { proc.reset(); }, processors);
    }

    /** Process `context` through all inner processors in sequence. */
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        procchain_detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept {
            if (context.usesSeparateInputAndOutputBlocks() && index != 0)
            {
                jassert (context.getOutputBlock().getNumChannels() == context.getInputBlock().getNumChannels());
                dsp::ProcessContextReplacing<typename ProcessContext::SampleType> replacingContext (context.getOutputBlock());
                replacingContext.isBypassed = (bypassed[index] || context.isBypassed);

                proc.process (replacingContext);
            }
            else
            {
                ProcessContext contextCopy (context);
                contextCopy.isBypassed = (bypassed[index] || context.isBypassed);

                proc.process (contextCopy);
            }
        },
                                          processors);
    }

    /** Process `context` through all inner processors in sequence. */
    template <typename FloatType>
    inline FloatType processSample (FloatType x) noexcept
    {
        procchain_detail::forEachInTuple ([&] (auto& proc, size_t /*index*/) noexcept {
            x = proc.processSample (x);
        },
                                          processors);

        return x;
    }

private:
    std::tuple<Processors...> processors;
    std::array<bool, sizeof...(Processors)> bypassed { {} };
};

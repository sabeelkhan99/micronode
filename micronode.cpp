#include "quickjs/quickjs.h"
#include <iostream>
#include <fstream>
#include <vector>
#include<cmath>

// Merge function to merge two sorted subarrays
void merge(std::vector<int> &arr, int left, int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Create temporary arrays
    std::vector<int> L(n1), R(n2);

    // Copy data to temporary arrays L[] and R[]
    for (int i = 0; i < n1; ++i)
        L[i] = arr[left + i];
    for (int i = 0; i < n2; ++i)
        R[i] = arr[mid + 1 + i];

    // Merge the temporary arrays back into arr[left..right]
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            ++i;
        }
        else
        {
            arr[k] = R[j];
            ++j;
        }
        ++k;
    }

    // Copy the remaining elements of L[], if any
    while (i < n1)
        arr[k++] = L[i++];

    // Copy the remaining elements of R[], if any
    while (j < n2)
        arr[k++] = R[j++];
}

// Merge Sort function
void mergeSort(std::vector<int> &arr, int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);      // Sort first half
        mergeSort(arr, mid + 1, right); // Sort second half
        merge(arr, left, mid, right);   // Merge the sorted halves
    }
}

// Function to expose to JavaScript
static JSValue js_mysort(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    if (argc != 1 || !JS_IsArray(ctx, argv[0]))
    {
        return JS_ThrowTypeError(ctx, "Expected an array as the argument");
    }

    // Convert JS array to C++ vector
    uint32_t length = 0;
    JSValue lengthValue = JS_GetPropertyStr(ctx, argv[0], "length");
    if (JS_IsNumber(lengthValue))
    {
        JS_ToUint32(ctx, &length, lengthValue);
    }
    JS_FreeValue(ctx, lengthValue);
    std::vector<int> arr(length);

    for (uint32_t i = 0; i < length; ++i)
    {
        JSValue val = JS_GetPropertyUint32(ctx, argv[0], i);
        if (!JS_IsNumber(val))
        {
            JS_FreeValue(ctx, val);
            return JS_ThrowTypeError(ctx, "Array must contain only numbers");
        }
        JS_ToInt32(ctx, &arr[i], val);
        JS_FreeValue(ctx, val);
    }

    // Sort the array using Merge Sort
    mergeSort(arr, 0, arr.size() - 1);

    // Convert C++ vector back to JS array
    JSValue result = JS_NewArray(ctx);
    for (uint32_t i = 0; i < arr.size(); ++i)
    {
        JS_SetPropertyUint32(ctx, result, i, JS_NewInt32(ctx, arr[i]));
    }

    return result;
}

static JSValue js_add(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    if (argc != 2 || !JS_IsNumber(argv[0]) || !JS_IsNumber(argv[1]))
    {
        return JS_ThrowTypeError(ctx, "Expected two numbers as arguments");
    }

    int32_t a, b;
    JS_ToInt32(ctx, &a, argv[0]);
    JS_ToInt32(ctx, &b, argv[1]);

    return JS_NewInt32(ctx, a + b);
}

// a^b
// e.g: 2^5 = 32

static JSValue js_power(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    if (argc != 2 || !JS_IsNumber(argv[0]) || !JS_IsNumber(argv[1]))
    {
        return JS_ThrowTypeError(ctx, "Expected two numbers as arguments");
    }

    int32_t a, b;
    JS_ToInt32(ctx, &a, argv[0]);
    JS_ToInt32(ctx, &b, argv[1]);

    return JS_NewInt32(ctx, pow(a,b));
}

// Console.log implementation
static JSValue js_console_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    const char *msg = JS_ToCString(ctx, argv[0]);
    std::cout << "[LOG]: " << msg << std::endl;
    JS_FreeCString(ctx, msg);
    return JS_UNDEFINED;
}

// Initialize modules
void initModules(JSContext *ctx)
{
    // Initialize console
    JSValue globalObj = JS_GetGlobalObject(ctx);

    // Initialise console
    JSValue console = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, console, "log", JS_NewCFunction(ctx, js_console_log, "log", 1));
    JS_SetPropertyStr(ctx, globalObj, "console", console);

    // Initialise utils
    JSValue utils = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, utils, "add", JS_NewCFunction(ctx, js_add, "add", 2));
    JS_SetPropertyStr(ctx, utils, "power", JS_NewCFunction(ctx, js_power, "power", 2));
    JS_SetPropertyStr(ctx, globalObj, "utils", utils);

    // Initialise sort
    JSValue algo = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, algo, "mergeSort", JS_NewCFunction(ctx, js_mysort, "mergeSort", 1));
    JS_SetPropertyStr(ctx, globalObj, "algo", algo);
    

    JS_FreeValue(ctx, globalObj);
}

// Main function
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << argc << std::endl;
        std::cerr << "Usage: micronode <file.js>" << std::endl;
        return 1;
    }

    // Create JS runtime and context
    JSRuntime *rt = JS_NewRuntime();
    JSContext *ctx = JS_NewContext(rt);

    initModules(ctx);

    // Read the JS file
    std::ifstream jsFile(argv[1]);
    std::string script((std::istreambuf_iterator<char>(jsFile)), std::istreambuf_iterator<char>());
    jsFile.close();

    // Execute the script
    JSValue result = JS_Eval(ctx, script.c_str(), script.length(), argv[1], JS_EVAL_TYPE_GLOBAL);
    if (JS_IsException(result))
    {
        JSValue exception = JS_GetException(ctx);
        const char *errorMsg = JS_ToCString(ctx, exception);
        std::cerr << "Error: " << errorMsg << std::endl;
        JS_FreeCString(ctx, errorMsg);
        JS_FreeValue(ctx, exception);
    }
    JS_FreeValue(ctx, result);

    // Clean up
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);

    return 0;
}

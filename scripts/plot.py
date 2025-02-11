import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import glob
import os


# Load all CSV files from the "result" folder
def load_data(folder):
    all_files = glob.glob(os.path.join(folder, "*.csv"))
    df_list = [pd.read_csv(f).assign(filename=os.path.basename(f)) for f in all_files]
    return pd.concat(df_list, ignore_index=True)


# Compute performance in FLOPs (Floating Point Operations per Second)
def compute_flops(df):
    df["FLOPs"] = df["NX"] * df["NT"] * 8  # Total floating-point operations
    df["GFLOPs/s"] = df["FLOPs"] / (df["MEAN"] * 1e-3) / 1e9  # Convert to GFLOPs per second
    return df


# Plot performance
def plot_performance_space(df):
    sns.set(style="whitegrid")
    plt.figure(figsize=(10, 6))
    sns.lineplot(x="NX", y="GFLOPs/s", hue="filename", marker="o", data=df)
    plt.xscale("linear")
    plt.xscale("linear")
    plt.xlabel("Space Division (NX)")
    plt.ylabel("Performance (GFLOPs/s)")
    plt.title("1D Heat Equation Performance")
    plt.legend(title="Dataset")
    plt.show()


def plot_performance_time(df):
    sns.set(style="whitegrid")
    plt.figure(figsize=(10, 6))
    sns.lineplot(x="NT", y="GFLOPs/s", hue="filename", marker="o", data=df)
    plt.xscale("linear")
    plt.xscale("linear")
    plt.xlabel("Time Division (NT)")
    plt.ylabel("Performance (GFLOPs/s)")
    plt.title("1D Heat Equation Performance")
    plt.legend(title="Dataset")
    plt.show()


if __name__ == "__main__":
    folder = "results"  # Folder containing CSV files
    df = load_data(folder)
    df = compute_flops(df)
    plot_performance_space(df)
    plot_performance_time(df)

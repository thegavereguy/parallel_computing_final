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

# Compute speedup between two selected files
def compute_speedup(df_base, df_compare):
    merged_df = df_base.merge(df_compare, on="NX", suffixes=("_base", "_compare"))
    merged_df["Speedup"] = merged_df["MEAN_base"] / merged_df["MEAN_compare"]
    return merged_df

# Plot speedup in 2D
def plot_speedup(df_base, df_compare):
    df_speedup = compute_speedup(df_base, df_compare)
    sns.set(style="whitegrid")
    plt.figure(figsize=(10, 6))
    sns.lineplot(x=df_speedup["NX"], y=df_speedup["Speedup"], marker="o")
    plt.xscale("log")
    plt.xlabel("Space Division (NX)")
    plt.ylabel("Speedup")
    plt.title(f"1D Heat Equation Speedup ({df_base['filename'].iloc[0]} vs {df_compare['filename'].iloc[0]})")
    plt.show()

if __name__ == "__main__":
    folder = "results"  # Folder containing CSV files
    df_all = load_data(folder)
    
    file_list = df_all["filename"].unique()
    print("Available files:")
    for i, file in enumerate(file_list):
        print(f"{i}: {file}")
    
    base_index = int(input("Select the index of the base file: "))
    compare_index = int(input("Select the index of the file to compare: "))
    
    df_base = df_all[df_all["filename"] == file_list[base_index]].copy()
    df_compare = df_all[df_all["filename"] == file_list[compare_index]].copy()
    
    plot_speedup(df_base, df_compare)

